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
    char nombre[100] = "Hospital Central";
    char direccion[150] = "Av. Principal, Chiquinquirá";
    char telefono[15] = "0261-1234567";
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

// === FUNCIONES DE ARCHIVO Y ACCESO ALEATORIO ===

bool verificarArchivo(const char* nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) return false;

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(header));
    archivo.close();

    return header.version == 1;
}

bool inicializarArchivo(const char* nombreArchivo) {
    std::ofstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) return false;

    ArchivoHeader header = {0, 1, 0, 1}; // versión 1
    archivo.write((char*)&header, sizeof(header));
    archivo.close();
    return true;
}

ArchivoHeader leerHeader(const char* nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    ArchivoHeader header = {};
    if (!archivo.is_open()) return header;

    archivo.read((char*)&header, sizeof(header));
    archivo.close();
    return header;
}

bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& header) {
    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
    if (!archivo.is_open()) return false;

    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(header));
    archivo.close();
    return true;
}

long calcularPosicion(int indice, size_t tamRegistro) {
    return sizeof(ArchivoHeader) + indice * tamRegistro;
}

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

void registrarPaciente() {
    Paciente p = {};
    cin.ignore();
    cout << "Nombre: "; cin.getline(p.nombre, 50);
    cout << "Apellido: "; cin.getline(p.apellido, 50);
    cout << "Cédula: "; cin.getline(p.cedula, 20);
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

