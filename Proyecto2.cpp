#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <vector>

using namespace std;

// === ESTRUCTURAS ===
struct ArchivoHeader {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
};

struct Hospital {
    char nombre[100] = "Hospital Universitario de Caracas";
    char direccion[150] = "Av. Minerva, UCV Caracas, Distrito Capital (Venezuela), 1040";
    char telefono[15] = "04126067652";
    int siguienteIDPaciente = 1;
    int siguienteIDDoctor = 1;
    int siguienteIDCita = 1;
    int siguienteIDConsulta = 1;
    int totalPacientesRegistrados = 0;
    int totalDoctoresRegistrados = 0;
    int totalCitasAgendadas = 0;
    int totalConsultasRealizadas = 0;
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
    char alergias[500];
    char observaciones[500];
    bool activo;
    int cantidadConsultas;
    int primerConsultaID;
    int cantidadCitas;
    int citasIDs[20];
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedulaProfesional[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];
    bool disponible;
    int cantidadPacientes;
    int pacientesIDs[50];
    int cantidadCitas;
    int citasIDs[30];
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

struct Cita {
    int id;
    int pacienteID;
    int doctorID;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[20];
    char observaciones[200];
    bool atendida;
    int consultaID;
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

struct HistorialMedico {
    int id;
    int pacienteID;
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int doctorID;
    float costo;
    int siguienteConsultaID;
    bool eliminado;
    time_t fechaRegistro;
    time_t fechaCreacion;
    time_t fechaModificacion;
   

};

//VALIDACIONES
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

//horario ocupado
bool horarioOcupado(int idDoctor, const char* fecha, const char* hora) {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) return false;

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(header));

    Cita c;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (!c.atendida && strcmp(c.estado, "Pendiente") == 0 &&
            c.idDoctor == idDoctor &&
            strcmp(c.fecha, fecha) == 0 &&
            strcmp(c.hora, hora) == 0) {
            archivo.close();
            return true;
        }
    }

    archivo.close();
    return false;
}

//Verificacion para saber si la cedua existe
bool cedulaExiste(const char* cedulaBuscada) {
    fstream archivo("pacientes.bin", ios::binary | ios::in);
    if (!archivo.is_open()) {
        cerr << "Error al abrir pacientes.bin\n";
        return false;
    }

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    Paciente paciente;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        long posicion = sizeof(ArchivoHeader) + i * sizeof(Paciente);
        archivo.seekg(posicion);
        archivo.read(reinterpret_cast<char*>(&paciente), sizeof(Paciente));

        if (!paciente.eliminado && strcmp(paciente.cedula, cedulaBuscada) == 0) {
            archivo.close();
            return true;
        }
    }

    archivo.close();
    return false;
}


// === FUNCIONES DE ARCHIVO Y ACCESO ALEATORIO ===
//Verificar archivo
bool verificarArchivo(const char* nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) return false;

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(header));
    archivo.close();

    return header.version == 1;
}

//Verificar header
bool verificarHeader(const char*nombreArchivo ) {
    ifstream archivo( nombreArchivo ,ios::binary | ios::in);
    if (!archivo.is_open()) {
        return false;
    }
    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
    archivo.close();
    if (archivo.fail() || header.version != 1) {
        
        return false;
    }
    return true;
}

//Inicializar archivo
bool inicializarArchivo(const char* nombreArchivo) {
    std::ofstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) return false;

    ArchivoHeader header = {0, 1, 0, 1}; // versión 1
    archivo.write((char*)&header, sizeof(header));
    archivo.close();
    return true;
}

//Archivo leerHeader
ArchivoHeader leerHeader(const char* nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    ArchivoHeader header = {};
    if (!archivo.is_open()) return header;

    archivo.read((char*)&header, sizeof(header));
    archivo.close();
    return header;
}

//Actualizar header
bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& header) {
    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
    if (!archivo.is_open()) return false;

    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(header));
    archivo.close();
    return true;
}

//Calcular posicion
long calcularPosicion(int indice, size_t tamRegistro) {
    return sizeof(ArchivoHeader) + indice * tamRegistro;
}

//Buscar indice por id
int buscarIndicePorID(const char* nombreArchivo, int idBuscado, size_t tamRegistro) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) return -1;

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(header));

    char* buffer = new char[tamRegistro];
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(buffer, tamRegistro);
        int id;
        memcpy(&id, buffer, sizeof(int));
        bool eliminado;
        memcpy(&eliminado, buffer + tamRegistro - sizeof(bool) - sizeof(time_t)*2, sizeof(bool));
        if (id == idBuscado && !eliminado) {
            delete[] buffer;
            archivo.close();
            return i;
        }
    }
    delete[] buffer;
    archivo.close();
    return -1;
}

//Mostrar header
void mostrarHeader(const char* nombreArchivo) {
    ArchivoHeader header = leerHeader(nombreArchivo);
    cout << "\n--- Header de " << nombreArchivo << " ---\n";
    cout << "Cantidad de registros: " << header.cantidadRegistros << "\n";
    cout << "Próximo ID: " << header.proximoID << "\n";
    cout << "Registros activos: " << header.registrosActivos << "\n";
    cout << "Versión: " << header.version << "\n";
}

//Agregar registro
template<typename T>
bool agregarRegistro(const char* nombreArchivo, T& nuevo) {
    ArchivoHeader header = leerHeader(nombreArchivo);

    nuevo.id = header.proximoID;     // ? Asigna el ID actual
    header.proximoID++;              // ? Aumenta para el siguiente

    nuevo.eliminado = false;
    nuevo.fechaCreacion = time(nullptr);
    nuevo.fechaModificacion = time(nullptr);

    std::ofstream archivo(nombreArchivo, std::ios::binary | std::ios::app);
    if (!archivo.is_open()) return false;

    archivo.write((char*)&nuevo, sizeof(T));
    archivo.close();

    header.cantidadRegistros++;
    header.registrosActivos++;

    return actualizarHeader(nombreArchivo, header); // ? Guarda el nuevo header
}

//Actualizar registro
template<typename T>
bool actualizarRegistro(const char* nombreArchivo, const T& modificado) {
    int indice = buscarIndicePorID(nombreArchivo, modificado.id, sizeof(T));
    if (indice == -1) return false;

    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
    if (!archivo.is_open()) return false;

    archivo.seekp(calcularPosicion(indice, sizeof(T)));
    archivo.write((char*)&modificado, sizeof(T));
    archivo.close();
    return true;
}

//Eliminar registro
template<typename T>
bool eliminarRegistro(const char* nombreArchivo, int id) {
    int indice = buscarIndicePorID(nombreArchivo, id, sizeof(T));
    if (indice == -1) return false;

    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
    if (!archivo.is_open()) return false;

    archivo.seekg(calcularPosicion(indice, sizeof(T)));
    T registro;
    archivo.read((char*)&registro, sizeof(T));

    registro.eliminado = true;
    registro.fechaModificacion = time(nullptr);

    archivo.seekp(calcularPosicion(indice, sizeof(T)));
    archivo.write((char*)&registro, sizeof(T));
    archivo.close();

    ArchivoHeader header = leerHeader(nombreArchivo);
    header.registrosActivos--;
    return actualizarHeader(nombreArchivo, header);
}

//listar registros activos
template<typename T>
void listarRegistrosActivos(const char* nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo.\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(header));
    T registro;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&registro, sizeof(T));
        if (!registro.eliminado) {
            std::cout << "ID: " << registro.id << " - Nombre: " << registro.nombre << "\n";
        }
    }
    archivo.close();
}

//PACIENTES

//Reiniciar el archivo de pacientes
void reiniciarArchivoPacientes() {
    ofstream archivo("pacientes.bin", ios::binary | ios::trunc);
    if (!archivo.is_open()) {
        cerr << "No se pudo crear pacientes.bin\n";
        return;
    }

    ArchivoHeader header = {0, 1, 0, 1}; // Reinicia contador de ID
    archivo.write((char*)&header, sizeof(header));
    archivo.close();

    cout << "Archivo pacientes.bin reiniciado. Todos los registros fueron eliminados.\n";
}

//Agregar paciente
void registrarPaciente(const char* cedulaInicial){
    Paciente p = {};
    char cedula[20];
    cin.ignore();
    cout << "Nombre: "; cin.getline(p.nombre, 50);
    cout << "Apellido: "; cin.getline(p.apellido, 50);
    strncpy(p.cedula, cedulaInicial, 20);
    cin.ignore();
    cout << "Edad: "; cin >> p.edad;
    cout << "Sexo (M/F): "; cin >> p.sexo;
    cin.ignore();
    cout << "Tipo de sangre: "; cin.getline(p.tipoSangre, 5);
    cout << "Teléfono: "; cin.getline(p.telefono, 15);
    cout << "Dirección: "; cin.getline(p.direccion, 100);
    cout << "Email: "; cin.getline(p.email, 50);
    cout << "Alergias: "; cin.getline(p.alergias, 500);
    cout << "Observaciones: "; cin.getline(p.observaciones, 500);
    p.activo = true;
    p.cantidadConsultas = 0;
    p.primerConsultaID = -1;
    p.cantidadCitas = 0;

    if (agregarRegistro("pacientes.bin", p)) {
        cout << "Paciente registrado con ID: " << p.id << "\n";
    } else {
        cout << "Error al registrar paciente.\n";
    }
}

//leer paciente por id
Paciente leerPacientePorID(int id) {
    int indice = buscarIndicePorID("pacientes.bin", id, sizeof(Paciente));
    if (indice == -1) return {};

    std::ifstream archivo("pacientes.bin", std::ios::binary);
    archivo.seekg(calcularPosicion(indice, sizeof(Paciente)));
    Paciente p;
    archivo.read((char*)&p, sizeof(Paciente));
    archivo.close();
    return p;
}

//Buscar paciente por nombre
void buscarPacientesPorNombre(const char* fragmento) {
    fstream archivo("pacientes.bin", ios::binary | ios::in);
    if (!archivo.is_open()) {
        cerr << "Error al abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    Paciente paciente;
    bool encontrado = false;

    cout << "\nPacientes que coinciden con \"" << fragmento << "\":\n";

    for (int i = 0; i < header.cantidadRegistros; i++) {
        long posicion = sizeof(ArchivoHeader) + i * sizeof(Paciente);
        archivo.seekg(posicion);
        archivo.read(reinterpret_cast<char*>(&paciente), sizeof(Paciente));

        if (!paciente.eliminado && strstr(paciente.nombre, fragmento) != nullptr) {
            cout << "ID: " << paciente.id << " | " << paciente.nombre << " " << paciente.apellido << "\n";
            encontrado = true;
        }
    }

    if (!encontrado) {
        cout << "No se encontraron pacientes con ese fragmento en el nombre.\n";
    }

    archivo.close();
}

//Buscar paciente por id
void buscarPacientePorID() {
    int id;
    cout << "Ingrese ID del paciente: ";
    cin >> id;
    Paciente p = leerPacientePorID(id);
    if (p.id == 0 || p.eliminado) {
        cout << "Paciente no encontrado.\n";
        return;
    }
    cout << "\nNombre: " << p.nombre << " " << p.apellido << "\n";
    cout <<"Apellido: " <<p.apellido << "\n";
    cout << "Cédula: " << p.cedula << "\n";
    cout << "Edad: " << p.edad << "\n";
	cout << "Sexo: " << p.sexo << "\n";
	cout << "Tipo de sangre: " << p.tipoSangre << "\n";
    cout << "Teléfono: " << p.telefono << "\n";
    cout << "Direccion: " << p.direccion<< "\n";
    cout << "Email: " << p.email << "\n";
    cout <<"Alergias: " <<p.alergias << "\n";
    cout <<"Observaciones: " <<p.observaciones << "\n";
}

//Buscar paciente por cedula
Paciente buscarPacientePorCedula(const char* cedulaBuscada) {
    fstream archivo("pacientes.bin", ios::binary | ios::in);
    Paciente paciente = {};
    
    if (!archivo.is_open()) {
        cerr << "Error al abrir pacientes.bin\n";
        paciente.id = -1;
        return paciente;
    }

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    for (int i = 0; i < header.cantidadRegistros; i++) {
        long posicion = sizeof(ArchivoHeader) + i * sizeof(Paciente);
        archivo.seekg(posicion);
        archivo.read(reinterpret_cast<char*>(&paciente), sizeof(Paciente));

        if (!paciente.eliminado && strcmp(paciente.cedula, cedulaBuscada) == 0) {
            archivo.close();
            return paciente;
        }
    }

    archivo.close();
    paciente.id = -1; // No encontrado
    return paciente;
}

//Acualizar datos del paciente
void actualizarPacienteInteractivo() {
    int id;
    cout << "ID del paciente a actualizar: ";
    cin >> id;
    Paciente p = leerPacientePorID(id);
    if (p.id == 0 || p.eliminado) {
        cout << "Paciente no encontrado.\n";
        return;
    }
    cin.ignore();
    cout << "Nuevo nombre (" << p.nombre << "): "; cin.getline(p.nombre, 50);
    cout << "Nuevo apellido (" << p.apellido << "): "; cin.getline(p.apellido, 50);
    cout << "Nueva cedula (" << p.cedula << "): "; cin.getline(p.cedula, 20);
    cout << "Nueva edad (" << p.edad <<"): "; cin>> p.edad;
    cout << "Nuevo sexo (" << p.sexo <<"): "; cin>> p.sexo;
    cin.ignore();
    cout << "Nuevo tipo de sangre (" << p.tipoSangre << "): "; cin.getline(p.tipoSangre, 5);
    cout << "Nuevo teléfono (" << p.telefono << "): "; cin.getline(p.telefono, 15);
    cout << "Nueva direccion (" << p.direccion << "): "; cin.getline(p.direccion, 100);
    cout << "Nueva alergia (" << p.alergias << "): "; cin.getline(p.alergias, 500);
    cout << "Nuevo email (" << p.email << "): "; cin.getline(p.email, 50);
    cout << "Nuevas observaciones (" << p.observaciones << "): "; cin.getline(p.observaciones, 500);
    p.fechaModificacion = time(nullptr);

    if (actualizarRegistro("pacientes.bin", p)) {
        cout << "Paciente actualizado.\n";
    } else {
        cout << "Error al actualizar.\n";
    }
}

//reordenar pacientes
void reordenarPacientes() {
    ifstream archivoOriginal("pacientes.bin", ios::binary);
    if (!archivoOriginal.is_open()) {
        cerr << "No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader headerOriginal;
    archivoOriginal.read((char*)&headerOriginal, sizeof(headerOriginal));

    vector<Paciente> pacientesActivos;
    Paciente p;

    for (int i = 0; i < headerOriginal.cantidadRegistros; i++) {
        archivoOriginal.read((char*)&p, sizeof(Paciente));
        if (!p.eliminado) {
            pacientesActivos.push_back(p);
        }
    }
    archivoOriginal.close();

    // Reasignar IDs consecutivos
    for (int i = 0; i < pacientesActivos.size(); i++) {
        pacientesActivos[i].id = i + 1;
    }

    // Reescribir archivo desde cero
    ofstream archivoNuevo("pacientes.bin", ios::binary | ios::trunc);
    if (!archivoNuevo.is_open()) {
        cerr << "No se pudo crear pacientes.bin\n";
        return;
    }

    ArchivoHeader nuevoHeader;
    nuevoHeader.cantidadRegistros = pacientesActivos.size();
    nuevoHeader.proximoID = pacientesActivos.size() + 1;
    nuevoHeader.registrosActivos = pacientesActivos.size();
    nuevoHeader.version = headerOriginal.version;

    archivoNuevo.write((char*)&nuevoHeader, sizeof(nuevoHeader));
    for (const auto& paciente : pacientesActivos) {
        archivoNuevo.write((char*)&paciente, sizeof(Paciente));
    }
    archivoNuevo.close();

    cout << "Pacientes reordenados automáticamente. Próximo ID: " << nuevoHeader.proximoID << "\n";
}

//eliminar paciente
void eliminarPacienteInteractivo() {
    int id;
    cout << "ID del paciente a eliminar: ";
    cin >> id;

    if (eliminarRegistro<Paciente>("pacientes.bin", id)) {
        cout << "Paciente eliminado lógicamente.\n";
        reordenarPacientes(); // ? ¡Aquí se reorganiza automáticamente!
    } else {
        cout << "No se pudo eliminar.\n";
    }
}


//DOCTORES

void reiniciarArchivoDoctores() {
ofstream archivo("doctores.bin", std::ios::binary | std::ios::trunc);
if (!archivo.is_open()) {
cerr << "No se pudo crear doctores.bin\n";
return;
}
ArchivoHeader header = {0, 1, 0, 1}; // Reinicia contador de ID
archivo.write((char*)&header, sizeof(header));
archivo.close();
cout << "Archivo doctores.bin reiniciado. Todos los registros fueron eliminados.\n";
}

void registrarDoctor() {
    Doctor d = {};
    cin.ignore();
    cout << "Nombre: "; cin.getline(d.nombre, 50);
    cout << "Apellido: "; cin.getline(d.apellido, 50);
    cout << "Cédula profesional: "; cin.getline(d.cedulaProfesional, 20);
    cout << "Especialidad: "; cin.getline(d.especialidad, 50);
    cout << "Años de experiencia: "; cin >> d.aniosExperiencia;
    cout << "Costo de consulta: "; cin >> d.costoConsulta;
    cin.ignore();
    cout << "Horario de atención: "; cin.getline(d.horarioAtencion, 50);
    cout << "Teléfono: "; cin.getline(d.telefono, 15);
   do {
        cout << "Email: ";
        cin.getline(d.email, 50);
        if (!validarEmail(d.email)) {
            cout << "? Email inválido. Debe contener '@' y al menos un '.' después.\n";
        }
    } while (!validarEmail(d.email));
    d.disponible = true;
    d.cantidadPacientes = 0;
    d.cantidadCitas = 0;

    if (agregarRegistro("doctores.bin", d)) {
        cout << "Doctor registrado con ID: " << d.id << "\n";
    } else {
        cout << "Error al registrar doctor.\n";
    }
}

Doctor buscarDoctorPorId(int id) {
    ifstream archivo("doctores.bin", std::ios::binary);
    Doctor resultado;
    resultado.id = 0;

    if (!archivo.is_open()) return resultado;

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(header));

    Doctor d;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado && d.id == id) {
            archivo.close();
            return d;
        }
    }
    
 
    
    

    archivo.close();
    return resultado;
}


void reordenarDoctores() {
    ifstream archivoOriginal("doctores.bin", ios::binary);
    if (!archivoOriginal.is_open()) {
        cerr << "No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader headerOriginal;
    archivoOriginal.read((char*)&headerOriginal, sizeof(headerOriginal));

    vector<Doctor> doctoresActivos;
    Doctor d;

    for (int i = 0; i < headerOriginal.cantidadRegistros; i++) {
        archivoOriginal.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado) {
            doctoresActivos.push_back(d);
        }
    }
    archivoOriginal.close();

    for (int i = 0; i < doctoresActivos.size(); i++) {
        doctoresActivos[i].id = i + 1;
    }

    ofstream archivoNuevo("doctores.bin", ios::binary | ios::trunc);
    if (!archivoNuevo.is_open()) {
        cerr << "No se pudo crear doctores.bin\n";
        return;
    }

    ArchivoHeader nuevoHeader;
    nuevoHeader.cantidadRegistros = doctoresActivos.size();
    nuevoHeader.proximoID = doctoresActivos.size() + 1;
    nuevoHeader.registrosActivos = doctoresActivos.size();
    nuevoHeader.version = headerOriginal.version;

    archivoNuevo.write((char*)&nuevoHeader, sizeof(nuevoHeader));
    for (const auto& doctor : doctoresActivos) {
        archivoNuevo.write((char*)&doctor, sizeof(Doctor));
    }
    archivoNuevo.close();

    cout << "Doctores reordenados automáticamente. Próximo ID: " << nuevoHeader.proximoID << "\n";
}

void eliminarDoctorInteractivo() {
    int id;
    cout << "ID del doctor a eliminar: ";
    cin >> id;

    if (eliminarRegistro<Doctor>("doctores.bin", id)) {
        cout << "Doctor eliminado lógicamente.\n";
        reordenarDoctores(); // ? reorganiza automáticamente
    } else {
        cout << "No se pudo eliminar.\n";
    }
}

void buscarDoctoresPorEspecialidad(const char* especialidadBuscada) {
   ifstream archivo("doctores.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir doctores.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(header));

    Doctor d;
    bool encontrados = false;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado && strcmp(d.especialidad, especialidadBuscada) == 0) {
            cout << "\nID: " << d.id << "\n";
            cout << "Nombre: " << d.nombre << " " << d.apellido << "\n";
            cout << "Teléfono: " << d.telefono << "\n";
            cout << "Email: " << d.email << "\n";
            encontrados = true;
        }
    }

    archivo.close();

    if (!encontrados) {
        cout << "No se encontraron doctores con esa especialidad.\n";
    }
}

bool asignarPacienteADoctor(int idDoctor, int idPaciente) {
    // Verificar si ya existe la asignación
    ifstream lectura("asignaciones.bin", ios::binary);
    Asignacion a;
    while (lectura.read((char*)&a, sizeof(Asignacion))) {
        if (a.idDoctor == idDoctor && a.idPaciente == idPaciente) {
            cout << "El paciente ya está asignado a este doctor.\n";
            lectura.close();
            return false;
        }
    }
    lectura.close();

    // Crear nueva asignación
    ofstream escritura("asignaciones.bin", ios::binary | ios::app);
    Asignacion nueva;
    nueva.idDoctor = idDoctor;
    nueva.idPaciente = idPaciente;
    nueva.fechaAsignacion = time(nullptr);
    escritura.write((char*)&nueva, sizeof(Asignacion));
    escritura.close();

    // Mostrar nombre del paciente
    ifstream pacientes("pacientes.bin", ios::binary);
    ArchivoHeader header;
    pacientes.read((char*)&header, sizeof(header));

    Paciente p;
    while (pacientes.read((char*)&p, sizeof(Paciente))) {
        if (!p.eliminado && p.id == idPaciente) {
            cout << "Paciente asignado: [" << p.id << "] " << p.nombre << " " << p.apellido << "\n";
            break;
        }
    }
    pacientes.close();

    return true;
}


void mostrarPacientesDeDoctor(int idDoctor) {
    ifstream asignaciones("asignaciones.bin", ios::binary);
    if (!asignaciones.is_open()) {
        cerr << "No se pudo abrir asignaciones.bin\n";
        return;
    }

    vector<int> idsPacientes;
    Asignacion a;
    while (asignaciones.read((char*)&a, sizeof(Asignacion))) {
        if (a.idDoctor == idDoctor) {
            idsPacientes.push_back(a.idPaciente);
        }
    }
    asignaciones.close();

    if (idsPacientes.empty()) {
        cout << "Este doctor no tiene pacientes asignados.\n";
        return;
    }

    ifstream pacientes("pacientes.bin", ios::binary);
    if (!pacientes.is_open()) {
        cerr << "No se pudo abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader header;
    pacientes.read((char*)&header, sizeof(header));

    Paciente p;
    cout << "\nPacientes asignados:\n";
    for (int id : idsPacientes) {
        pacientes.clear();
        pacientes.seekg(sizeof(ArchivoHeader)); // volver al inicio de los registros

        while (pacientes.read((char*)&p, sizeof(Paciente))) {
            if (!p.eliminado && p.id == id) {
                cout << "[" << p.id << "] " << p.nombre << " " << p.apellido << "\n";
                break;
            }
        }
    }

    pacientes.close();
}
