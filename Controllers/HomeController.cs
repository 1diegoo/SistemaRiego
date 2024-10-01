using System.Diagnostics;
using Microsoft.AspNetCore.Mvc;
using SistemaRiego.Models;

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
    public IActionResult ActualizarValores(int Id, string? Nm, float Sh, float Ah, float Tm, float Lm)
    {
        var datos = new SensorData {
            AccionadorId = Id,
            AccionadorNombre = Nm ?? "Accionador default",
            HumedadSuelo = Sh,
            HumedadAire = Ah, 
            Temperatura = Tm,
            Luminosidad = Lm };
        // Actualizar el estado de los sensores para cada accionador
        if (datos.AccionadorId == 1)
        {
            SensorDataManager.SetData(1, datos);
            if (datos.HumedadSuelo < 30 && !estadoRiego1)
            {
                estadoRiego1 = true;
                Task.Delay(10000).ContinueWith(_ => estadoRiego1 = false); // Desactivar riego después de 10 segundos

                var respBody = $"{{\"ok\": {(estadoRiego1 ? 1 : 0)}}}";

                HttpContext.Response.Headers.Remove("Server");

                return new ContentResult
                {
                    Content = respBody,
                    ContentType = "application/json",
                    StatusCode = 200
                };
            }
        }
        else if (datos.AccionadorId == 2)
        {
            SensorDataManager.SetData(2, datos);
            if (datos.HumedadSuelo < 30 && !estadoRiego2)
            {
                estadoRiego2 = true;
                Task.Delay(10000).ContinueWith(_ => estadoRiego2 = false); // Desactivar riego después de 10 segundos

                var respBody = $"{{\"ok\": {(estadoRiego2 ? 1 : 0)}}}";

                HttpContext.Response.Headers.Remove("Server");

                return new ContentResult
                {
                    Content = respBody,
                    ContentType = "application/json",
                    StatusCode = 200
                };
            }
        }
        HttpContext.Response.Headers.Remove("Server");

        return Ok();
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
    private static Dictionary<int, SensorData> _datos = new Dictionary<int, SensorData>
    {
        { 1, new SensorData { AccionadorNombre = "Accionador 1", HumedadSuelo = 45, HumedadAire = 60, Temperatura = 22, Luminosidad = 75 } },
        { 2, new SensorData { AccionadorNombre = "Accionador 2", HumedadSuelo = 48, HumedadAire = 58, Temperatura = 24, Luminosidad = 80 } }
    };

    public static void SetData(int id, SensorData data)
    {
        _datos[id] = data;
    }

    public static SensorData GetData(int id)
    {
        return _datos.ContainsKey(id) ? _datos[id] : new SensorData();
    }
}