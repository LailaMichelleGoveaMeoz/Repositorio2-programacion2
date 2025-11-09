#include <iomanip> //manipulacion de formatodo de salida, 
#include <iostream>//salida y entrada estandar
#include <cstring> //funciones para manupilar acdenas estilo char
#include <locale.h>// configurar acentos
using namespace std;

// Estructuras
struct HistorialMedico {
    int idConsulta;
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor;
    float costoConsulta;
};

struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[20];
    char observaciones[200];
    bool atendida;
};

struct Paciente {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad;
    char sexo;
    char tipoSangre[5];
    char telefono[15];
    char direccion[100];
    char email[50];
    HistorialMedico* historial; // apunta hacia la estructura de historial medico para hacer un array dinamico de la estructura HistorialMedico (cada paciente va a tener su propio histo)
    int cantidadConsultas;
    int capacidadConsultas;
    int capacidadHistorial; 
    int cantidadHistorial;
    int* citasAgendadas; // Puntero para poder hacer un array dinamico porque la cantidad de citas puede aumentar con el tiempo
    int cantidadCitas;
    int capacidadCitas;
    char alergias[500];
    char observaciones[500];
    bool activo; // inidcar si el paciente sigue en el sistema
};


struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];
    int* pacientesAsignados;// puntero para hacer array dinamico de pacientes asignados porque el tamano puede aumentar con el tiempo
    int cantidadPacientes;
    int capacidadPacientes;
    int* citasAgendadas; // perimite hacer arreglo dinamico porque el tamano puede aumentar con el tiempo
    int cantidadCitas;
    int capacidadCitas;
    bool disponible; // 
};
struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];
    Paciente* pacientes; //puntero que prmite crear arrays dinamicos de los pacientes registrados,apunta a la estructura paciente
    int cantidadPacientes;
    int capacidadPacientes;
    int siguienteIdPaciente; // asigna los IDs de los pacientes y luego va incrementando a medida que se vayan registrando nuevos pacientes
    int siguienteIdConsulta;
    Doctor* doctores; //puntero que permite crear un arrays dinamico de los doctores que se vayan a registrar, apunta a la estrutra doctor
    int cantidadDoctores;
    int capacidadDoctores;
    int siguienteIdDoctor;// asigna los ID y luego va icrementado
    Cita* citas; // puntero que permite rear un array dinamico de laas citas registradas
    int cantidadCitas;
    int capacidadCitas;
    int siguienteIdCita; // asigna lso ID y luego va incrementado

};


// Inicializar hospital
Hospital* inicializarHospital(const char* nombre, const char* direccion, const char* telefono){// el tipo de la funcion es Hospital* (direccion hospital), la funcion tiene como parametro nobre,direccion,telefono
    Hospital* hospital = new Hospital;// se crea Hospital en memoria dinamica, guarda esa direccion en el ptr y en el return se entraga la direccion
    strncpy(hospital->nombre, nombre, 100);// strncpy para copiar cadenas de caracteres
    strncpy(hospital->direccion, direccion, 150);// se utiliza minuscula porque quiero acceder especificamente al hospital que cree en memoria dinamica (con el que estoy trabajando)
    strncpy(hospital->telefono, telefono, 15);
    hospital->capacidadPacientes = 10;
    hospital->cantidadPacientes = 0;
    hospital->pacientes = new Paciente[hospital->capacidadPacientes];//crea array dinamico de pacientes con capacidad de 10, new porque puede aumentar de tamano
    hospital->siguienteIdPaciente = 1;// Id del primero en registrase 
    hospital->siguienteIdConsulta = 1;
    hospital->capacidadDoctores = 10;
hospital->cantidadDoctores = 0;
hospital->doctores = new Doctor[hospital->capacidadDoctores]; //crea array dinamico de pacientes con capacidad de 10, new porque puede aumentar de tamano
hospital->siguienteIdDoctor = 1;
hospital->capacidadCitas = 20;
hospital->cantidadCitas = 0;
hospital->siguienteIdCita = 1;
hospital->citas = new Cita[hospital->capacidadCitas];//crea array dinamico de citas con capacidad de 20, new porque puede aumentar de tamano


    return hospital;//retorna la direccion guardada en el puntero hospital
}

// b) Buscar paciente por cedula
Paciente* buscarPacientePorCedula(Hospital* hospital, const char* cedulaBuscada){//funcion que me va a servir para varios pacientes
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strcmp(hospital->pacientes[i].cedula, cedulaBuscada) == 0) {//strcmp se utiliza para comprar cadenas de caracteres, caracter por caracter. si son iguales me da 0
            return &hospital->pacientes[i];
        }
    }
    return nullptr; 
}

//cedula existe
bool cedulaExiste(Hospital* hospital, const char* cedulaBuscada) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strcmp(hospital->pacientes[i].cedula, cedulaBuscada) == 0) {
            return true;
        }
    }
    return false;
}

// c) Buscar paciente por Id
Paciente* buscarPacientePorId(Hospital* hospital, int id) {// paciente* porque va a retornar un puntero, la direccion del paciente que busco
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) {
            return &hospital->pacientes[i];
        }
    }
    return NULL;
}

//  a) Crear paciente
Paciente* crearPaciente(Hospital* hospital, const char* nombre, const char* apellido, const char* cedula,
                        int edad, char sexo,
                        const char* tipoSangre, const char* telefono, const char* direccion,
                        const char* email, const char* alergias, const char* observaciones){
						
//redimensionar arreglos de pacientes
    if (hospital->cantidadPacientes >= hospital->capacidadPacientes) { //compara la cantidad de pacientes para saber si esta full
        int nuevaCapacidad = hospital->capacidadPacientes * 2;// nueva capcidad va a ser el doble de la vieja
        Paciente* nuevoArray = new Paciente[nuevaCapacidad]; // vuelve al nuevo array dinamico
        for (int i = 0; i < hospital->cantidadPacientes; i++) { //itera para copiar los datos del viejo al nuevo
            nuevoArray[i] = hospital->pacientes[i];
        }
        delete[] hospital->pacientes;// libero el arreglo anterior, puntero de un arreglo
        hospital->pacientes = nuevoArray; // reutilizo el puntero
        hospital->capacidadPacientes = nuevaCapacidad;
    }

    Paciente& nuevo = hospital->pacientes[hospital->cantidadPacientes];//me permite registrar un nuevo paciente. se esta creando una referencia al espacio en donde se va a guardar el nuevo paciente, que es el espacio libre en la cantidad del arreglo
    nuevo.id = hospital->siguienteIdPaciente++;
    strncpy(nuevo.nombre, nombre, 50);// destino, origen, tamano-1
    strncpy(nuevo.apellido, apellido, 50);
    strncpy(nuevo.cedula, cedula, 20);
    nuevo.edad = edad;
    nuevo.sexo = sexo;
    strncpy(nuevo.tipoSangre, tipoSangre, 5);
    strncpy(nuevo.telefono, telefono, 15);
    strncpy(nuevo.direccion, direccion, 100);
    strncpy(nuevo.email, email, 50);
    strncpy(nuevo.alergias, alergias, 500);
    strncpy(nuevo.observaciones, observaciones, 500);
    nuevo.activo = true;
    nuevo.capacidadConsultas = 5;
    nuevo.cantidadHistorial = 0;
    nuevo.capacidadHistorial=5;
    nuevo.historial = new HistorialMedico[nuevo.capacidadHistorial]; // se le esta creando su array dinmico de historial medico porque cada paciente
    nuevo.capacidadCitas = 5;
    nuevo.cantidadCitas = 0;
    nuevo.citasAgendadas = new int[nuevo.capacidadCitas];
    hospital->cantidadPacientes++;
    return &nuevo;
 
}


//d) buscar pacientes por nombre
void buscarPacientesPorNombre(Hospital* h, const char* fragmento) { // puntero alhospital que contiene todos los pacientes regsitrados
    cout << "\nPacientes que coinciden con \"" << fragmento << "\":\n";
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (strstr(h->pacientes[i].nombre, fragmento) != nullptr) { // strstr para buscar una subcadena en una cadena, en este caso un frangmento del nombre
            cout << "ID: " << h->pacientes[i].id << " | " << h->pacientes[i].nombre << " " << h->pacientes[i].apellido << "\n";
        }
    }
}


//e) Actualizar paciente
bool actualizarPaciente(Hospital* hospital, int id) {// bool para saber si ya el paciente se encuentra regristrado
    Paciente* p = buscarPacientePorId(hospital, id); // p es un puntero que va a guardar la direccion del paciente encontrado
    if (!p) return false;// si no se encuentra por id el paciente no esta regristrado

    cin.ignore();
    cout << "Nombre actual: " << p->nombre << "\nNuevo nombre: "; cin.getline(p->nombre, 50);
    cout << "Apellido actual: " << p->apellido << "\nNuevo apellido: "; cin.getline(p->apellido, 50);
    cout << "Edad actual: " << p->edad << "\nNueva edad: "; cin >> p->edad;
    cout << "Sexo actual: " << p->sexo << "\nNuevo sexo (M/F): "; cin >> p->sexo;
    cin.ignore();
    cout << "Tipo de sangre actual: " << p->tipoSangre << "\nNuevo tipo de sangre: "; cin.getline(p->tipoSangre, 5);
    cout << "Teléfono actual: " << p->telefono << "\nNuevo teléfono: "; cin.getline(p->telefono, 15);
    cout << "Dirección actual: " << p->direccion << "\nNueva dirección: "; cin.getline(p->direccion, 100);
    cout << "Email actual: " << p->email << "\nNuevo email: "; cin.getline(p->email, 50);
    cout << "Alergias actuales: " << p->alergias << "\nNuevas alergias: "; cin.getline(p->alergias, 500);
    cout << "Observaciones actuales: " << p->observaciones << "\nNuevas observaciones: "; cin.getline(p->observaciones, 500);
    return true;
}


//f) eliminar pciente
bool eliminarPaciente(Hospital* h, int id) {
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (h->pacientes[i].id == id) {
            delete[] h->pacientes[i].historial;// aqui solo se libera el historial

            // Eliminar citas
            for (int j = 0; j < h->cantidadCitas; j++) {//verfica que la cita es del paciente que se va a eliminar
                if (h->citas[j].idPaciente == id) {//verifica que las citas contengan el id del paciente que ingrese
                    h->citas[j].idPaciente = -1;// es para decir que esa cita ya no esta asignada a nadie, -1 se utiliza comunmente para indicar que no haya nada asignado porque no hay ningun id que sea -1
                    strcpy(h->citas[j].estado, "Cancelada"); //para que salgaen estado que fue cancelada//strcpy no pone limite de la cadena
                }
            }

            // Remover de doctores
            for (int d = 0; d < h->cantidadDoctores; d++) {
                Doctor& doc = h->doctores[d]; // crea una referencia al doctor que estoy iterando
                for (int p = 0; p < doc.cantidadPacientes; p++) {// estoy viendo la cantidad de pacientes que tiene ese docto
                    if (doc.pacientesAsignados[p] == id) {// verifica que el paciente este asignado al doctro
                        for (int k = p; k < doc.cantidadPacientes - 1; k++) {//p es la posicion del id que quiero eliminar, como k y p es lo mismo por lo tanto en el siguiente argumento es k<(menor) cantidadPacientes -1 (k=1, cantidadPaciente=4,va desde 1 hasta el tres (4-1=3))
                            doc.pacientesAsignados[k] = doc.pacientesAsignados[k + 1]; // para rellenar el hueco que me quedan en el arreglo luego de que se haya eliminado el ID, el siguiente paciente ahora ocupara el puesto dell anteorior
                        }
                        doc.cantidadPacientes--;// reduce el contador de pacientes registrados
                        break;// para salir de inmediato el bucle
                    }
                }
            }

            // Compactar arreglo
            for (int k = i; k < h->cantidadPacientes - 1; k++) { //hace lo mismo que doc.cantidadPaciente pero para cantidadPaciente Hospiatl
                h->pacientes[k] = h->pacientes[k + 1];
            }
            h->cantidadPacientes--; 
            return true;
        }
    }
    return false;
}

//g) lista de todos los pacientes
void listarPacientes(Hospital* hospital) {
    cout << "\nListado de Pacientes\n";
    cout << left << setw(5) << "ID"
         << setw(15) << "Nombre" // setw nos permite acomoda como se va a ver lo que se quiera imprimir cuando se imprima (lo arregla de manera visual)
         << setw(15) << "Apellido"
         << setw(15) << "Cedula de identidad"
         << setw(6) << "Edad"
         << setw(10) << "Consultas" << "\n";

    cout << string(66, '-') << "\n"; // Línea divisoria, es la linea que divide los titulos de la info

    for (int i = 0; i < hospital->cantidadPacientes; i++) { 
        Paciente& p = hospital->pacientes[i]; // se crea una referencia de cada paciente que vaya iterando
        cout << left << setw(5) << p.id
             << setw(15) << p.nombre
             << setw(15) << p.apellido
             << setw(15) << p.cedula
             << setw(6) << p.edad
             << setw(10) << p.cantidadConsultas << "\n";
    }
}

// h) agregarHistorial
bool agregarHistorialMedico(Paciente* paciente, const char* fecha, const char* hora,
                            const char* diagnostico, const char* tratamiento,
                            const char* medicamentos, int idDoctor, float costo) {
 
    if (!paciente) return false;

    // Verificar si hay que duplicar el arreglo
    if (paciente->cantidadHistorial >= paciente->capacidadHistorial) {
        int nuevaCapacidad = paciente->capacidadHistorial * 2;
        HistorialMedico* nuevoHistorial = new HistorialMedico[nuevaCapacidad];// arreglo dinamico con la nueva capacidad

        // Copiar datos existentes
        for (int i = 0; i < paciente->cantidadHistorial; i++) {
            nuevoHistorial[i] = paciente->historial[i];
        }

        // Liberar memoria anterior
        delete[] paciente->historial;
        paciente->historial = nuevoHistorial;
        paciente->capacidadHistorial = nuevaCapacidad;
    }

    // Agregar nueva entrada
    HistorialMedico& h = paciente->historial[paciente->cantidadHistorial];// creando una referencia llamada h al nuevo espacio donde se va a guardar el próximo historial médico del paciente, cantidadHistorial
    h.idConsulta = paciente->cantidadHistorial + 1;// asigna el id de la consulta segun la cantidad del historial

    strncpy(h.fecha, fecha, 10); h.fecha[10] = '\0';//se agrega si no estoy segura que el origen es menor al maximo de caracteres
    strncpy(h.hora, hora, 5); h.hora[5] = '\0';
    strncpy(h.diagnostico, diagnostico, 199); h.diagnostico[199] = '\0';
    strncpy(h.tratamiento, tratamiento, 199); h.tratamiento[199] = '\0';
    strncpy(h.medicamentos, medicamentos, 149); h.medicamentos[149] = '\0';

    h.idDoctor = idDoctor; //asignando el id del doctor que atendio la cita
    h.costoConsulta = costo; 

    paciente->cantidadHistorial++; //cantidad de historial va a ir incrementando a medida que uno vaya teniendo mas consultas
    paciente->cantidadConsultas++; // incrementa el contador cada vez que un paciente tenga una consulta

    return true;
}

//LE muestra el historial medico al pacinete
void mostrarHistorialMedico(Paciente* paciente) {
    if (paciente->cantidadHistorial == 0) {
        cout << "Este paciente no tiene historial médico registrado.\n";
        return;
    }

    cout << "\nHistorial Médico del Paciente:\n";
    for (int i = 0; i < paciente->cantidadHistorial; i++) {
        HistorialMedico& h = paciente->historial[i]; // se hace una referencia del historial con el que se este trabajndo, va iterando, y se va a imprimir toda la info
        cout << "\nConsulta #" << h.idConsulta << "\n";
        cout << "Fecha: " << h.fecha << "   Hora: " << h.hora << "\n";
        cout << "Diagnóstico: " << h.diagnostico << "\n";
        cout << "Tratamiento: " << h.tratamiento << "\n";
        cout << "Medicamentos: " << h.medicamentos << "\n";
        cout << "ID Doctor: " << h.idDoctor << "\n";
        cout << "Costo: $" << h.costoConsulta << "\n";
    }
}

//DOCTORES
//Cedula existe para doctor
bool cedulaExisteDoctor(Hospital* hospital, const char* cedulaBuscada) {
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].cedula, cedulaBuscada) == 0) {
            return true;
        }
    }
    return false;
}

//A) crear doctor
Doctor* crearDoctor(Hospital* hospital, const char* nombre, const char* apellido, const char* cedula,
                    const char* especialidad, int experiencia, float costo,
                    const char* horario, const char* telefono, const char* email) {
    // Verificar si hay espacio suficiente
    if (hospital->cantidadDoctores >= hospital->capacidadDoctores) {// redimensionar
        int nuevaCapacidad = hospital->capacidadDoctores * 2;
        Doctor* nuevoArray = new Doctor[nuevaCapacidad];

        for (int i = 0; i < hospital->cantidadDoctores; i++) {
            nuevoArray[i] = hospital->doctores[i];
        }

        delete[] hospital->doctores;
        hospital->doctores = nuevoArray;
        hospital->capacidadDoctores = nuevaCapacidad;
    }

    // Crear nuevo doctor
    Doctor& d = hospital->doctores[hospital->cantidadDoctores];// referencia que apunta hacien el espacio libre donde yo voy a poder guardar aldoctor
    d.id = hospital->siguienteIdDoctor++;

    strncpy(d.nombre, nombre, 49); d.nombre[49] = '\0';
    strncpy(d.apellido, apellido, 49); d.apellido[49] = '\0';
    strncpy(d.cedula, cedula, 19); d.cedula[19] = '\0';
    strncpy(d.especialidad, especialidad, 49); d.especialidad[49] = '\0';
    d.aniosExperiencia = experiencia;
    d.costoConsulta = costo;
    strncpy(d.horarioAtencion, horario, 49); d.horarioAtencion[49] = '\0';
    strncpy(d.telefono, telefono, 14); d.telefono[14] = '\0';
    strncpy(d.email, email, 49); d.email[49] = '\0';
    d.disponible = true;

    // Inicializar arreglos dinámicos
    d.capacidadPacientes = 5;
    d.cantidadPacientes = 0;
    d.pacientesAsignados = new int[d.capacidadPacientes];

    d.capacidadCitas = 5;
    d.cantidadCitas = 0;
    d.citasAgendadas = new int[d.capacidadCitas];

    hospital->cantidadDoctores++;
    return &d;
}


//b) Buscar doctor por ID
Doctor* buscarDoctorPorId(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) return &hospital->doctores[i];//retorno directamente el espacio de memoria del doctor al que le pertenece al id
    }
    return nullptr;
}

//c) Buscar doctor por especialidad
void buscarDoctoresPorEspecialidad(Hospital* hospital, const char* especialidad) {
    cout << "\nDoctores con especialidad en " << especialidad << ":\n";
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i]; //se crea un referencia del doctor que se va a trabajar en esa iteracion
        if (strcmp(d.especialidad, especialidad) == 0) {
            cout << "ID: " << d.id << " - " << d.nombre << " " << d.apellido << "\n";
        }
    }
}

//d) asignar paciente a doctor
bool asignarPacienteADoctor(Doctor* doctor, int idPaciente) {
    if (doctor->cantidadPacientes >= doctor->capacidadPacientes) {
        int nuevaCapacidad = doctor->capacidadPacientes * 2; 
        int* nuevoArray = new int[nuevaCapacidad];
        for (int i = 0; i < doctor->cantidadPacientes; i++) {
            nuevoArray[i] = doctor->pacientesAsignados[i];
        }
        delete[] doctor->pacientesAsignados;
        doctor->pacientesAsignados = nuevoArray;
        doctor->capacidadPacientes = nuevaCapacidad;
    }

    doctor->pacientesAsignados[doctor->cantidadPacientes++] = idPaciente;
    return true; // va sumando la cantidad de pacienteque se le asugna al doctor y asu vez guarda el id del paciente en el arreglo que guarda que paciente va a tener el doctro
}

//Ver los pacientes asignado al doctor
void verPacientesDeDoctor(Hospital* hospital, Doctor* doctor) {
    cout << "\n Paciente asignado al doctor es: " << doctor->nombre << " " << doctor->apellido << ":\n";
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        int idPaciente = doctor->pacientesAsignados[i];
        Paciente* p = buscarPacientePorId(hospital, idPaciente);// utiliza la funcion buscar por id para darme los datos
        if (p) {
            cout << "ID: " << p->id << " - " << p->nombre << " " << p->apellido << "\n";
        }
    }
}

//lista de todos los doctores
void listarDoctores(Hospital* hospital) {
    cout << "\nLista  de Doctores:\n";
    cout << left << setw(5) << "ID"
         << setw(20) << "Nombre"
         << setw(20) << "Apellido"
         << setw(20) << "Especialidad"
         << setw(15) << "Telefono" << "\n";

    cout << string(80, '-') << "\n";

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        cout << left << setw(5) << d.id
             << setw(20) << d.nombre
             << setw(20) << d.apellido
             << setw(20) << d.especialidad
             << setw(15) << d.telefono << "\n";
    }
}

//Eliminar doctor
bool eliminarDoctor(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) {
            delete[] hospital->doctores[i].pacientesAsignados;
            delete[] hospital->doctores[i].citasAgendadas;

            for (int j = i; j < hospital->cantidadDoctores - 1; j++) {
                hospital->doctores[j] = hospital->doctores[j + 1];
            }
            hospital->cantidadDoctores--;
            return true;
        }
    }
    return false;
}

//GESTION DE CITAS
//Agendar citas
bool agendarCita(Hospital* hospital, int idPaciente, int idDoctor, const char* fecha, const char* hora, const char* motivo) {
    if (hospital->cantidadCitas >= hospital->capacidadCitas) {
        int nuevaCapacidad = hospital->capacidadCitas * 2;
        Cita* nuevoArray = new Cita[nuevaCapacidad];
        for (int i = 0; i < hospital->cantidadCitas; i++) {
            nuevoArray[i] = hospital->citas[i];
        }
        delete[] hospital->citas;
        hospital->citas = nuevoArray;
        hospital->capacidadCitas = nuevaCapacidad;
    }

    Cita& c = hospital->citas[hospital->cantidadCitas];
    c.id = hospital->siguienteIdCita++;
    c.idPaciente = idPaciente;
    c.idDoctor = idDoctor;
    strncpy(c.fecha, fecha, 10); c.fecha[10] = '\0';
    strncpy(c.hora, hora, 5); c.hora[5] = '\0';
    strncpy(c.motivo, motivo, 149); c.motivo[149] = '\0';
    strcpy(c.estado, "Pendiente");
    strcpy(c.observaciones, "");
    c.atendida = false; //la estoy creando

    hospital->cantidadCitas++;
    return true;
}

//cancelar las citas
bool cancelarCita(Hospital* hospital, int idCita) {
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].id == idCita && !hospital->citas[i].atendida) { // comparo si el id de la cita que estoy iterando es igual al que puse en la funcion y siveo a ver si la cita ya fue atendida nko puedo cancelar una ya atendida
            strcpy(hospital->citas[i].estado, "Su cita ha sido cancelada");
            hospital->citas[i].atendida = false;
            return true;
        }
    }
    return false;
    
}//Para saber si el horario esta ocupado
bool horarioOcupado(Hospital* hospital, int idDoctor, const char* fecha, const char* hora) {
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i]; //refrencia de la cita que estoy trabajando
        if (c.idDoctor == idDoctor &&
            strcmp(c.fecha, fecha) == 0 &&
            strcmp(c.hora, hora) == 0 &&
            strcmp(c.estado, "Pendiente") == 0) {//si compara todo eso y da 0, ya hay una cita en ese horario
            return true; // Ya hay una cita pendiente en ese horario
        }
    }
    return false;
}

//Ver todas citas de los pacientes
void verCitasDePaciente(Hospital* hospital, int idPaciente) {
    cout << "\nCitas del paciente:\n";
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idPaciente == idPaciente) {
            Cita& c = hospital->citas[i];
            cout << "ID: " << c.id << " | Fecha: " << c.fecha << " " << c.hora
                 << " | Motivo: " << c.motivo << " | Estado: " << c.estado << "\n";
        }
    }
}

//Ver todas las citas de los doctores
void verCitasDeDoctor(Hospital* hospital, int idDoctor) {
    cout << "\nCitas del doctor:\n";
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == idDoctor) {
            Cita& c = hospital->citas[i];
            cout << "ID: " << c.id << " | Fecha: " << c.fecha << " " << c.hora
                 << " | Motivo: " << c.motivo << " | Estado: " << c.estado << "\n";
        }
    }
}
void verCitasPorFecha(Hospital* hospital, const char* fecha) {
    cout << "\nCitas para la fecha " << fecha << ":\n";
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (strcmp(hospital->citas[i].fecha, fecha) == 0) {
            Cita& c = hospital->citas[i];
            cout << "ID: " << c.id << " | Hora: " << c.hora
                 << " | Motivo: " << c.motivo << " | Estado: " << c.estado << "\n";
        }
    }
}
void verCitasPendientes(Hospital* hospital) {
    cout << "\nCitas pendientes:\n";
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (strcmp(hospital->citas[i].estado, "Pendiente") == 0) {//comparo el estado que me salga pendiente
            Cita& c = hospital->citas[i];
            cout << "ID: " << c.id << " | Fecha: " << c.fecha << " " << c.hora
                 << " | Motivo: " << c.motivo << "\n";
        }
    }
    
}// Aternder cita
bool atenderCita(Hospital* hospital, int idCita, const char* observaciones) {
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];

        if (c.id == idCita && strcmp(c.estado, "Pendiente") == 0) {//iguala la cita de la iteracion con la cita que coloque y el estado de la cita esta pendiente sale atendida
            // Marcar como atendida
            strcpy(c.estado, "Atendida");
            strncpy(c.observaciones, observaciones, 199);
            c.observaciones[199] = '\0';
            c.atendida = true;

            // Buscar paciente y doctor
            Paciente* p = buscarPacientePorId(hospital, c.idPaciente);
            Doctor* d = buscarDoctorPorId(hospital, c.idDoctor);
            //busca y comparar que existan lso ids

            if (p && d) {
                agregarHistorialMedico(p, c.fecha, c.hora, c.motivo, "Tratamiento aplicado", "Medicamentos recetados", d->id, d->costoConsulta);//se compara que ambos id existen para agregar la info al historial del paciente
            }

            return true;
        }
    }

    return false;
}

//funcion para validar la fecha en YYYY-MM-DD

bool validarFecha(const char* fecha){
    int ano, mes, dia;
    if(strlen(fecha)!=10){// strlen se utiliza para indicar si la cadena es mas corta o mas larga de 10 digitos no es valida
        return false;
    }
//Para verificar el formato con guiones
    if(fecha[4]!='-' || fecha[7]!='-') {// verfica que los guines esten en las posiciones correctas
        return false;
    }
//verificamos que el numero corresponda 
for(int i=0; i<10;i++) {
    if (i==4 || i==7 ) //saltos en el caracter 4 y 7
continue;//saltamos los guiones
    if (fecha[i] < '0' || fecha[i]>'9') {// verifica que solo se ingresen numeros
    return false;
      }
}

//extraemos año,mes y dia 

ano =    (fecha[0]-'0')* 1000 + //hago la resta porque los caracteres tienen valores numerico internos (ASCII), "2"-"0"= 50-48=2
         (fecha[1]-'0')* 100 +
         (fecha[2]-'0')* 10 +
         (fecha[3]-'0');
mes =    (fecha[5]-'0')* 10 + (fecha[6]-'0');
dia =    (fecha[8]-'0')* 10 + (fecha[9]- '0');


//validar rangos 

    if ( ano < 1900 || ano > 2100) return false;
    if (mes < 1 || mes > 12) return false;
    if (dia < 1 || dia > 31) return false;

//verificar dias segun mes

if (mes == 4 || mes == 6 || mes == 9 ||mes == 11) {
    if (dia>30) return false;
} else if (mes==2) {
    bool esBisiesto = (ano % 4 == 0 && ano % 100 != 0) || (ano% 400 == 0); // aplica la reggla de como saber si un ano es bisiesto
        if (esBisiesto) {
            if (dia > 29) return false;
        } else {
            if (dia > 28) return false;
        }
       
    }
    
      return true;
}

//funcion para validar la hora en formato HH:MM

bool validarHora(const char* hora) {
    if (strlen(hora) != 5) return false;{
    }
    if (hora[2] != ':') return false;{
    }
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue; // saltar el ':'
        if (hora[i] < '0' || hora[i] > '9') {return false;
    }
    }
    int HH = (hora[0] - '0') * 10 + (hora[1] - '0');
    int MM = (hora[3] - '0') * 10 + (hora[4] - '0');
    if (HH < 0 || HH > 23) return false;
    if (MM < 0 || MM > 59) return false;
    return true;
}


//funcion para validar el email
bool validarEmail(const char* email) {
    if(email==nullptr|| strlen (email)==0){//asegura que el email no este vacio //strlen maneja la logitud de cadena
        return false;
    }
    bool tienearroba=false;
    bool tienepunto=false;
    for(int i=0;email[i]!='0';i++){//va a reccorrer cada caracter del email que se ingrese hasta llegar al final
        if(email[i]=='@'){
            tienearroba=true;
        } else if(email[i]=='.' && tienearroba){
            tienepunto=true;
        }
    }
    return tienearroba && tienepunto;//return true
}


// Limpieza de memoria
void destruirHospital(Hospital* hospital) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        delete[] hospital->pacientes[i].historial;
        delete[] hospital->pacientes[i].citasAgendadas;
   

       
    }
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
    delete[] hospital->doctores[i].pacientesAsignados;
    delete[] hospital->doctores[i].citasAgendadas;
}


    delete[] hospital->pacientes;
    delete[] hospital->doctores;
    delete hospital;
}


// Menú principal
int main() {
	setlocale(LC_ALL,"");
    Hospital* hospital = inicializarHospital("Hospital Universitario de Caracas", "Av. Minerva, UCV Caracas, Distrito Capital (Venezuela), 1040", "+58 04126067652");
    cout << "Hospital: " << hospital->nombre << endl;
cout << "Dirección: " << hospital->direccion << endl;
cout << "Teléfono: " << hospital->telefono << endl;
    int opcion = 0;

    while (opcion != 4) {
        cout << "\n--- MENU PRINCIPAL ---\n";
        cout << "1. Gestion de Pacientes\n";
        cout << "2. Gestion de Doctores \n";
        cout << "3. Gestion de Citas \n";
        cout << "4. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int subopcion = -1;
                while (subopcion != 0) {
                    cout << "\n--- Gestión de Pacientes ---\n";
                    cout << "1. Registrar nuevo paciente\n";
                    cout << "2. Buscar paciente por ID\n";
                    cout << "3. Actualizar datos del paciente\n";
                    cout << "4. Listar todos los pacientes\n";
                    cout << "5. Eliminar paciente\n";
                    cout << "6. Buscar paciente por cédula\n";
                    cout << "7. Agregar historial médico\n";
                    cout << "8. Ver historial médico\n";
                    cout << "9. Buscar paciente por nombre\n";

                    cout << "0. Volver al menú principal\n";
                    cout << "Seleccione una opción: ";
                    cin >> subopcion;

                    switch (subopcion) {
                        case 1: {
                            char nombre[50], apellido[50], cedula[20];
int edad;
char sexo;
char tipoSangre[5], telefono[15], direccion[100], email[50];
char alergias[500], observaciones[500];
                            cin.ignore();
                            cout << "Nombre: "; cin.getline(nombre, 50);
                            cout << "Apellido: "; cin.getline(apellido, 50);
                            cout << "Cédula: "; cin.getline(cedula, 20);
                            if (cedulaExiste(hospital, cedula)) {
    cout << "Ya existe un paciente registrado con esa cédula.\n";
    break; // o return, según tu estructura
}
                            cout << "Edad: "; cin >> edad;
                             cout << "Sexo (M/F): "; cin >> sexo;
                             cin.ignore();
                             cout<<"Alergias: " ;cin.getline(alergias, 500);
                             cout<<"Tipo de Sangre: " ;cin.getline(tipoSangre, 5);
                             cout<<"Telefono: " ;cin.getline(telefono, 15);
                             cout<<"Direccion: " ;cin.getline(direccion, 100);
                             
                             do{
                             cout<<"Email: " ;cin.getline(email, 50);
                             if (!validarEmail(email)){
                             	cout << "Email inválido. Intente de nuevo.\n";
							 }
						} while (!validarEmail(email));
						
                             cout<<"observaciones: " ;cin.getline(observaciones, 500);
                             cin.ignore();
                            Paciente* nuevo = crearPaciente(hospital, nombre, apellido, cedula, edad, sexo, tipoSangre, telefono, direccion, email, alergias, observaciones);
if (nuevo) {
    cout << "Paciente registrado con ID: " << nuevo->id << "\n";
}
   break;
                        }

                        case 2: {
                            int id;
                            cout << "Ingrese el ID del paciente: ";
                            cin >> id;
                            Paciente* p = buscarPacientePorId(hospital, id);
                            if (p) {
                                cout << "\nPaciente encontrado:\n";
                                cout << "Nombre: " << p->nombre << " " << p->apellido << "\n";
                                cout << "Cédula: " << p->cedula << "\n";
                                cout << "Edad: " << p->edad << "\n";
                                cout << "Sexo: " << p->sexo << "\n";
                                cout << "Tipo de sangre: " << p->tipoSangre << "\n";
                                cout << "Teléfono: " << p->telefono << "\n";
                                cout << "Email: " << p->email << "\n";
                                cout << "Consultas registradas: " << p->cantidadConsultas << "\n";
                            } else {
                                cout << "Paciente no encontrado.\n";
                            }
                            break;
                        }

                        case 3: {
                            int id;
                            cout << "Ingrese el ID del paciente a actualizar: ";
                            cin >> id;
                            if (actualizarPaciente(hospital, id)) {
                                cout << "Datos actualizados correctamente.\n";
                            } else {
                                cout << "No se pudo actualizar el paciente.\n";
                            }
                            break;
                        }

                        case 4:
                            listarPacientes(hospital);
                            break;

                        case 5: {
                            int id;
                            cout << "Ingrese el ID del paciente a eliminar: ";
                            cin >> id;
                            if (eliminarPaciente(hospital, id)) {
                                cout << "Paciente eliminado correctamente.\n";
                            } else {
                                cout << "No se pudo eliminar el paciente.\n";
                            }
                            break;
                        }
                        case 6: {
    char cedulaBuscada[20];
    cin.ignore();
    cout << "Ingrese la cédula del paciente: ";
    cin.getline(cedulaBuscada, 20);

    Paciente* p = buscarPacientePorCedula(hospital, cedulaBuscada);
    if (p) {
        cout << "\nPaciente encontrado:\n";
        cout << "ID: " << p->id << "\n";
        cout << "Nombre: " << p->nombre << " " << p->apellido << "\n";
        cout << "Cédula: " << p->cedula << "\n";
        cout << "Edad: " << p->edad << "\n";
        cout << "Sexo: " << p->sexo << "\n";
        cout << "Tipo de sangre: " << p->tipoSangre << "\n";
        cout << "Teléfono: " << p->telefono << "\n";
        cout << "Dirección: " << p->direccion << "\n";
        cout << "Email: " << p->email << "\n";
        cout << "Alergias: " << p->alergias << "\n";
        cout << "Observaciones: " << p->observaciones << "\n";
        cout << "Consultas registradas: " << p->cantidadConsultas << "\n";
    } else {
        cout << "Paciente no encontrado.\n";
    }
    break;
}
case 7: {

    int id;
    cout << "\nIngrese el ID del paciente: ";
    cin >> id;
    Paciente* p = buscarPacientePorId(hospital, id);
    if (!p) {
        cout << "Paciente no encontrado.\n";
        break;
    }

    // Variables de entrada
    char fecha[11], hora[6], diagnostico[200], tratamiento[200], medicamentos[150];
    int idDoctor;
    float costo;

    cin.ignore(); // limpiar buffer

    cout << "\n--- Registro de Historial Médico ---\n";

    cout << "Fecha (YYYY-MM-DD): ";
    cin.getline(fecha, 11);
    cout << "? Fecha ingresada: " << fecha << "\n";

    cout << "Hora (HH:MM): ";
    cin.getline(hora, 6);
    cout << "? Hora ingresada: " << hora << "\n";

    cout << "Diagnóstico (máx 199 caracteres): ";
    cin.getline(diagnostico, 199);
    cout << "? Diagnóstico ingresado: " << diagnostico << "\n";

    cout << "Tratamiento (máx 199 caracteres): ";
    cin.getline(tratamiento, 199);
    cout << "? Tratamiento ingresado: " << tratamiento << "\n";

    cout << "Medicamentos recetados (máx 149 caracteres): ";
    cin.getline(medicamentos, 149);
    cout << "? Medicamentos ingresados: " << medicamentos << "\n";

    cout << "ID del doctor que atendió: ";
    cin >> idDoctor;
    cout << "? ID doctor: " << idDoctor << "\n";

    cout << "Costo de la consulta: ";
    cin >> costo;
    cout << "? Costo ingresado: $" << costo << "\n";

    // Confirmación antes de guardar
    cout << "\nGuardando historial médico...\n";

    bool exito = agregarHistorialMedico(p, fecha, hora, diagnostico, tratamiento, medicamentos, idDoctor, costo);
    if (exito) {
        cout << "? Historial médico agregado correctamente.\n";
    } else {
        cout << "? No se pudo agregar el historial (posible error de memoria).\n";
    }

    break;
}

case 8: {
    int id;
    cout << "Ingrese el ID del paciente: ";
    cin >> id;
    Paciente* p = buscarPacientePorId(hospital, id);
    if (!p) {
        cout << "Paciente no encontrado.\n";
        break;
    }

    mostrarHistorialMedico(p);
    break;
}
case 9: {
    char nombreBuscado[50];
    cin.ignore();
    cout << "Ingrese el nombre o parte del nombre a buscar: ";
    cin.getline(nombreBuscado, 50);
    buscarPacientesPorNombre(hospital, nombreBuscado);
    break;
}




                        case 0:
                            cout << "Volviendo al menú principal...\n";
                            break;

                        default:
                            cout << "Opción inválida. Intente nuevamente.\n";
                            
                    }
                }
                break;
        }

           case 2: {
    int opcionDoctor = -1;
    while (opcionDoctor != 8) {
        cout << "\n--- Menú de Doctores ---\n";
        cout << "1. Registrar nuevo doctor\n";
        cout << "2. Buscar doctor por ID\n";
        cout << "3. Buscar doctores por especialidad\n";
        cout << "4. Asignar paciente a doctor\n";
        cout << "5. Ver pacientes asignados a doctor\n";
        cout << "6. Listar todos los doctores\n";
        cout << "7. Eliminar doctor\n";
        cout << "8. Volver al menú principal\n";
        cout << "Seleccione una opción: ";
        cin >> opcionDoctor;

        switch (opcionDoctor) {
            case 1: {
                char nombre[50], apellido[50], cedula[20], especialidad[50], horario[50], telefono[15], email[50];
                int experiencia;
                float costo;

                cin.ignore();
                cout << "Nombre: "; cin.getline(nombre, 50);
                cout << "Apellido: "; cin.getline(apellido, 50);
                cout << "Cédula: "; cin.getline(cedula, 20);
                if (cedulaExisteDoctor(hospital, cedula)) {
    cout << "Ya existe un doctor registrado con esa cédula.\n";
    break;} // o return, según tu estructura
                cout << "Especialidad: "; cin.getline(especialidad, 50);
                cout << "Años de experiencia: "; cin >> experiencia;
                cout << "Costo de consulta: "; cin >> costo;
                cin.ignore();
                cout << "Horario de atención: "; cin.getline(horario, 50);
                cout << "Teléfono: "; cin.getline(telefono, 15);
                do{
                             cout<<"Email: " ;cin.getline(email, 50);
                             if (!validarEmail(email)){
                             	cout << "Email inválido. Intente de nuevo.\n";
							 }
						} while (!validarEmail(email));

                Doctor* d = crearDoctor(hospital, nombre, apellido, cedula, especialidad, experiencia, costo, horario, telefono, email);
                if (d) {
                    cout << "Doctor registrado con ID: " << d->id << "\n";
                } else {
                    cout << "No se pudo registrar el doctor.\n";
                }
                break;
            }

            case 2: {
                int id;
                cout << "Ingrese el ID del doctor: ";
                cin >> id;
                Doctor* d = buscarDoctorPorId(hospital, id);
                if (d) {
                    cout << "\nDoctor encontrado:\n";
                    cout << "Nombre: " << d->nombre << " " << d->apellido << "\n";
                    cout << "Especialidad: " << d->especialidad << "\n";
                    cout << "Teléfono: " << d->telefono << "\n";
                    cout << "Email: " << d->email << "\n";
                } else {
                    cout << "? Doctor no encontrado.\n";
                }
                break;
            }

            case 3: {
                char especialidad[50];
                cin.ignore();
                cout << "Ingrese la especialidad a buscar: ";
                cin.getline(especialidad, 50);
                buscarDoctoresPorEspecialidad(hospital, especialidad);
                break;
            }

            case 4: {
                int idDoctor, idPaciente;
                cout << "ID del doctor: "; cin >> idDoctor;
                cout << "ID del paciente a asignar: "; cin >> idPaciente;

                Doctor* d = buscarDoctorPorId(hospital, idDoctor);
                Paciente* p = buscarPacientePorId(hospital, idPaciente);

                if (!d || !p) {
                    cout << "? Doctor o paciente no encontrado.\n";
                    break;
                }

                if (asignarPacienteADoctor(d, idPaciente)) {
                    cout << "? Paciente asignado correctamente.\n";
                } else {
                    cout << "? No se pudo asignar el paciente.\n";
                }
                break;
            }

            case 5: {
                int id;
                cout << "Ingrese el ID del doctor: ";
                cin >> id;
                Doctor* d = buscarDoctorPorId(hospital, id);
                if (d) {
                    verPacientesDeDoctor(hospital, d);
                } else {
                    cout << "? Doctor no encontrado.\n";
                }
                break;
            }

            case 6: {
                listarDoctores(hospital);
                break;
            }

            case 7: {
                int id;
                cout << "Ingrese el ID del doctor a eliminar: ";
                cin >> id;
                if (eliminarDoctor(hospital, id)) {
                    cout << "? Doctor eliminado correctamente.\n";
                } else {
                    cout << "? No se encontró el doctor.\n";
                }
                break;
            }

            case 8:
                cout << "Volviendo al menú principal...\n";
                break;

            default:
                cout << "? Opción inválida. Intente nuevamente.\n";
        }
    }
    break;
}

case 3: {
    int opcionCita = 0;
    do {
        cout << "\n--- Menú de Citas ---\n";
        cout << "1. Agendar nueva cita\n";
        cout << "2. Cancelar cita\n";
        cout << "3. Atender cita\n";
        cout << "4. Ver citas de un paciente\n";
        cout << "5. Ver citas de un doctor\n";
        cout << "6. Ver citas de una fecha\n";
        cout << "7. Ver citas pendientes\n";
        cout << "8. Volver al menú principal\n";
        cout << "Seleccione una opción: ";
        cin >> opcionCita;
        cin.ignore();

        switch (opcionCita) {
            case 1: {
                int idPaciente, idDoctor;
                char fecha[11], hora[6], motivo[150];
                cout << "ID del paciente: "; cin >> idPaciente;
                cout << "ID del doctor: "; cin >> idDoctor;
                cin.ignore();
                do {
        cout << "Fecha (YYYY-MM-DD): "; cin.getline(fecha, 11);
        if (!validarFecha(fecha)) {
            cout << "? Fecha inválida. Intente de nuevo.\n";
        }
    } while (!validarFecha(fecha));

                do {
        cout << "Hora (HH:MM): "; cin.getline(hora, 6);
        if (!validarHora(hora)) {
            cout << "? Hora inválida. Intente de nuevo.\n";
        }
    } while (!validarHora(hora));

                cout << "Motivo: "; cin.getline(motivo, 150);

                if (horarioOcupado(hospital, idDoctor, fecha, hora)) {
                    cout << "? El doctor ya tiene una cita pendiente en ese horario.\n";
                } else {
                    if (agendarCita(hospital, idPaciente, idDoctor, fecha, hora, motivo)) {
                        int nuevoId = hospital->siguienteIdCita - 1; //El ID de la última cita que se acaba de registrar es uno menos que el siguiente ID disponible
                        cout << "? Cita agendada correctamente. ID: " << nuevoId << "\n";
                    } else {
                        cout << "? No se pudo agendar la cita.\n";
                    }
                }
                break;
            }

            case 2: {
                int idCita;
                cout << "ID de la cita a cancelar: ";
                cin >> idCita;
                if (cancelarCita(hospital, idCita))
                    cout << "? Cita cancelada.\n";
                else
                    cout << "? No se pudo cancelar la cita.\n";
                break;
            }

            case 3: {
                int idCita;
                char observaciones[200];
                cout << "ID de la cita a atender: ";
                cin >> idCita;
                cin.ignore();
                cout << "Observaciones: ";
                cin.getline(observaciones, 200);

                if (atenderCita(hospital, idCita, observaciones))
                    cout << "? Cita atendida.\n";
                else
                    cout << "? No se pudo atender la cita.\n";
                break;
            }

            case 4: {
                int idPaciente;
                cout << "ID del paciente: ";
                cin >> idPaciente;
                verCitasDePaciente(hospital, idPaciente);
                break;
            }

            case 5: {
                int idDoctor;
                cout << "ID del doctor: ";
                cin >> idDoctor;
                verCitasDeDoctor(hospital, idDoctor);
                break;
            }

            case 6: {
                char fecha[11];
                cin.ignore();
                cout << "Ingrese la fecha (YYYY-MM-DD): ";
                cin.getline(fecha, 11);
                verCitasPorFecha(hospital, fecha);
                break;
            }

            case 7: {
                verCitasPendientes(hospital);
                break;
            }

            case 8:
                cout << "Volviendo al menú principal...\n";
                break;

            default:
                cout << "? Opción inválida. Intente nuevamente.\n";
        }
    } while (opcionCita != 8);
    break;
}



case 4:
    destruirHospital(hospital);
    cout << "Memoria liberada. ¡Hasta luego!\n";
    break;


            default:
                cout << "Opción inválida. Intente nuevamente.\n";
        }
    }

    return 0;

}
