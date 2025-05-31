[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=19677066&assignment_repo_type=AssignmentRepo)
# Proyecto final

## Integrantes

 Juan David Bernal Bernal

## Nombre del proyecto: 
 
  Contador de cabinas

## Documentación
 
 ## Introduccion
   
   Crearemos un sistema basado en radiofrecuencia para enviar un dato desde la cabina donde sea recepcionado al paso de cada estación procesandolo a una base de datos cuyo propósito será llevar a detalle los ciclos de trabajo para generar un plan de mantenimiento más detallado optimizando la gestión de repuestos, permitiendo tener control sobre la predicción de fallas.

 ## Nuestra necesidad 

  Contamos con un sistema de transporte excepcional lo cual implica llevar el control de ciclos por cabina para programar sus rutinas de mantenimiento y rediseñar el plan de mantenimiento de acuerdo al registro real.

 ## Como se realiza actualmete?

 Se realiza de manera teórica y se estima de acuerdo a las horas de operación programadas, su cálculo se realiza de acuerdo al promedio de ciclos que complete al cabo de hacer un recorrido completo por las cuatro estaciones y en cada uno se cuentan 12 ciclos, una cabina por dia en 18 horas se estima que realiza un aproximado de 52 ciclos por dia, y el registro se lleva ha llevado de esta forma.

 ## Configuracio inicial

 Se usará el módulo transreceptor LoRa SX1278 junto con el microcontrolador PIC18F45K22 mediante la comunicación sincrónica usando el protocolo SPI (Serial Peripheral Interface) cuya particularidad de configuración se ajusta para el receptor y emisor, la detección del paso a través de una entrada o salida de estación, se tomará de acuerdo a un patrón de dispositivos magnéticos que permitan reconocer una secuencia en software que realizara una secuencia y direccionara la información a el modulo receptor más cercano, allí este dato mediante una raspberry se convertirá en una base formada a través de SQL que permitirá indexar cada uno de los registros en una base de excel.

 ## Objetivo general
 Desarrollar un sistema basado en radiofrecuencia que registre con precisión los ciclos de trabajo de las cabinas en cada estación para optimizar la planificación del mantenimiento, mejorar la gestión de repuestos y permitir la predicción de fallas.

  ## Objetivos específicos

 - Técnicos y de Implementación:

    - Diseñar e implementar un sistema de detección mediante dispositivos magnéticos que identifiquen       inequívocamente el paso de cada cabina por las estaciones.
    - Configure la comunicación SPI entre el microcontrolador PIC18F45K22 y el módulo transreceptor LoRa SX1278.
    - Desarrollar el software necesario para procesar la secuencia de detección y transmitir los datos al receptor más cercano.
    - Implementar una base de datos SQL en Raspberry Pi que almacene eficientemente todos los registros de ciclos.
    - Lograr Crear una interfaz de exportación a Excel para facilitar el análisis y la gestión de los datos.


 - Operativos y de Gestión:

    - Sustituir el sistema actual de estimación teórica por un registro preciso y automatizado de ciclos reales.
    - Reduzcir el margen de error en el conteo de ciclos a menos del 1%.
    - Establecer un sistema de alertas para cuando las cabinas se aproximan a umbrales críticos de ciclos que requieren mantenimiento.
    - Garantizar el funcionamiento continuo del sistema durante las 18 horas diarias de operación por cada cabina en circulacion.


 - Estratégicos:

   - Rediseñar el plan de mantenimiento basado en datos reales de uso, optimizando los intervalos de servicio.
   - Implementar un modelo predictivo de fallas basado en patrones históricos de ciclos y eventos.
   - Reducir los tiempos de inactividad por mantenimiento correctivo en al menos un 30%.
   - Optimizar el inventario de repuestos según los datos reales de desgaste y ciclos de trabajo.
   - Aumentar la vida útil de los componentes críticos mediante mantenimiento preventivo oportuno.


 - Económicos:

  - Reducir los costos de mantenimiento correctivo en un 25% durante el primer año.
  - Optimizar el inventario de repuestos, reduciendo el capital inmovilizado en al menos un 15%.
  - Lograr un retorno de inversión del proyecto en un plazo máximo de 2 años mediante la reducción de costos operativos.


 - Documentación y Medición:

  - Implementar un sistema de informes automáticos (base de datos), que muestren claves métricas como ciclos por cabina, por estación y por período.
  - Desarrollar un tablero que permita visualizar en tiempo real el estado de las cabinas y su proximidad a ciclos de mantenimiento.
  - Documentar completamente el sistema para facilitar su mantenimiento y posibles actualizaciones futuras.

  ## Justificacion

  Este proyecto de sistema de monitoreo de ciclos mediante radiofrecuencia se justifica por la necesidad crítica de transitar desde un modelo de mantenimiento basado en estimaciones teóricas hacia uno fundamentado en datos reales y precisos.
  Actualmente, el mantenimiento de las cabinas se planifica con base en cálculos aproximados (52 ciclos diarios estimados), lo que genera incertidumbre en la programación de las intervenciones y puede resultar en dos escenarios igualmente problemáticos: mantenimiento prematuro, que desperdicia recursos y genera paradas innecesarias, o mantenimiento tardío, que aumenta el riesgo de fallas imprevistas y costosas.
  La implementación de este sistema automatizado permitirá:

  - Optimización de recursos : El registro exacto de ciclos reducirá significativamente los costos de mantenimiento correctivo y la gestión de inventarios de repuestos.
  - Aumento de disponibilidad : La predicción precisa de fallas minimizará los tiempos de inactividad no programados, maximizando la disponibilidad del sistema de transporte.
  - Extensión de vida útil : Un mantenimiento basado en el uso real de los componentes prolongará su vida útil, retrasando inversiones en reposición.
  - Mejora continua : Los datos históricos recopilados permitirán perfeccionar constantemente los planes de mantenimiento, adaptándolos a las condiciones reales de operación.
  - Toma de decisiones informada : La administración de mantenimiento contará con información precisa para planificar presupuestos, recursos humanos y materiales necesarios para el mantenimiento.
  
  ## Que es un ciclo de cabina 
   
  Es la apertura y desembrague de una cabina en la estacion dejando de sujetarse del cable tractor donde la misma se sujeta y alli transita mediante sistemas de traccion prpios de la estacion y el control sobre el movimiento de la cabina se da por llantas que friccionan el vehiculo y lo desplazan a lo largo de la pista de transito.

  A continuacion se mostrara como se realiza un ciclo por cabina

    <video src="Ciclo.mp4" controls="controls" style="max-width: 730px;">
    </video>
  



## Diagramas



## Conclusiones


<!-- Crear una carpeta src e incluir en ella los códigos y/o el proyecto de mplab-->
