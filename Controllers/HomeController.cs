using System.Diagnostics;
using Microsoft.AspNetCore.Mvc;
using SistemaRiego.Models;
using Microsoft.AspNetCore.SignalR;
using Microsoft.Extensions.DependencyInjection; // Para usar `HttpContext.RequestServices`

namespace SistemaRiego.Controllers;

public class HomeController : Controller
{
    private readonly ILogger<HomeController> _logger;

    // Variable para almacenar el estado del riego de cada accionador
    private static bool estadoRiego1 = false;
    private static bool estadoRiego2 = false;

    public HomeController(ILogger<HomeController> logger)
    {
        _logger = logger;
    }

    public IActionResult Index()
    {
        return View();
    }

    // Método para recibir datos del prototipo para cada accionador
    [HttpGet]
    [Route("api/Updt")]
    public async Task<IActionResult> ActualizarValores(int Id, string? Nm, float Sh, float Ah, float Tm, float Lm)
    {
        // Validar que el ID sea válido (por ejemplo, 1 o 2)
        if (Id < 1 || Id > 2)
        {
            return BadRequest(new { error = "ID del accionador inválido." });
        }

        // Validar rangos de datos
        if (Sh < 0 || Sh > 100 || Ah < 0 || Ah > 100 || Tm < -50 || Tm > 50 || Lm < 0 || Lm > 100)
        {
            return BadRequest(new { error = "Datos fuera de rango." });
        }

        var datos = new SensorData
        {
            AccionadorId = Id,
            AccionadorNombre = Nm ?? $"Accionador {Id}",
            HumedadSuelo = Sh,
            HumedadAire = Ah,
            Temperatura = Tm,
            Luminosidad = Lm
        };

        // Guardar datos en el sistema
        SensorDataManager.SetData(Id, datos);

        // Activar riego si la humedad del suelo está por debajo de 30%
        if (datos.HumedadSuelo < 30)
        {
            await ActivarRiegoAsync(Id); // Usa un método asincrónico para manejar el riego
        }

        // Notificar a los clientes conectados (frontend)
        var hubContext = HttpContext.RequestServices.GetService<IHubContext<SensorHub>>();
        if (hubContext != null)
        {
            await hubContext.Clients.All.SendAsync("ActualizarInterfaz", datos);
        }

        // Devolver éxito al prototipo
        return Ok(new { success = true });
    }

    // Método asincrónico para manejar el riego
    private async Task ActivarRiegoAsync(int id)
    {
        if (id == 1)
        {
            estadoRiego1 = true;
            await Task.Delay(10000); // Espera 10 segundos
            estadoRiego1 = false;
        }
        else if (id == 2)
        {
            estadoRiego2 = true;
            await Task.Delay(10000); // Espera 10 segundos
            estadoRiego2 = false;
        }
    }

    // Método para enviar datos de sensores al frontend
    [HttpGet]
    [Route("api/obtenerDatosSensores")]
    public IActionResult ObtenerDatosSensores()
    {
        var datosSensores1 = SensorDataManager.GetData(1);
        var datosSensores2 = SensorDataManager.GetData(2);

        return Json(new
        {
            humedadSuelo1 = datosSensores1.HumedadSuelo,
            humedadAire1 = datosSensores1.HumedadAire,
            temperatura1 = datosSensores1.Temperatura,
            luminosidad1 = datosSensores1.Luminosidad,
            nombreAccionador1 = datosSensores1.AccionadorNombre,

            humedadSuelo2 = datosSensores2.HumedadSuelo,
            humedadAire2 = datosSensores2.HumedadAire,
            temperatura2 = datosSensores2.Temperatura,
            luminosidad2 = datosSensores2.Luminosidad,
            nombreAccionador2 = datosSensores2.AccionadorNombre,

            estadoRiego1 = estadoRiego1,
            estadoRiego2 = estadoRiego2
        });
    }

    // Acción para mostrar la vista de configuración de cada accionador
    public IActionResult ConfigurarAccionador(int id)
    {
        ViewBag.AccionadorId = id;
        return View();
    }
}

// Clase para representar los datos del sensor para cada accionador
// Nombres recortados para reducir la cantidad de bytes necesaria para
// realizar un metodo GET desde el accionador
public class SensorData
{
    /*public SensorData(int id, string? nm, float sh, float ah, float tm, float lm)
    {
        AccionadorId = id;
        AccionadorNombre = nm;
        HumedadSuelo = sh;
        HumedadAire = ah;
        Temperatura = tm;
        Luminosidad = lm;
    }*/

    public int AccionadorId { get; set; } // Identificador del accionador (1 o 2)
    public string? AccionadorNombre { get; set; } = "Accionador";
    public float HumedadSuelo { get; set; }
    public float HumedadAire { get; set; }
    public float Temperatura { get; set; }
    public float Luminosidad { get; set; }
}

// Clase para manejar los datos de los sensores de cada accionador
public static class SensorDataManager
{
    private static Dictionary<int, SensorData> _datos = new Dictionary<int, SensorData>();

    public static void SetData(int id, SensorData data)
    {
        _datos[id] = data;
    }

    public static SensorData GetData(int id)
    {
        return _datos.ContainsKey(id) ? _datos[id] : new SensorData();
    }
}