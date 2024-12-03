var builder = WebApplication.CreateBuilder(args);

// Agregar servicios al contenedor
builder.Services.AddControllersWithViews();

// Agregar servicios de SignalR
builder.Services.AddSignalR();

var app = builder.Build();

// Configurar el pipeline de solicitudes HTTP
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Home/Error");
    app.UseHsts(); // Configuración para HTTPS en producción
}

app.UseHttpsRedirection(); // Redirección a HTTPS
app.UseStaticFiles(); // Habilitar archivos estáticos como CSS y JS

app.UseRouting(); // Habilitar enrutamiento
app.UseAuthorization(); // Autorización (puedes omitir si no la usas)

// Configurar la ruta para el hub de SignalR
app.MapHub<SensorHub>("/sensorHub"); // IMPORTANTE: Debe estar antes de MapControllerRoute

// Configurar la ruta para controladores y vistas
app.MapControllerRoute(
    name: "default",
    pattern: "{controller=Home}/{action=Index}/{id?}");

app.Run();