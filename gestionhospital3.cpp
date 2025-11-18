#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <locale.h>// configurar acentos

using namespace std;

#ifndef CONSTANTES_HPP
#define CONSTANTES_HPP

namespace Rutas {
    static const char* HOSPITAL     = "hospital.bin";
    static const char* PACIENTES    = "pacientes.bin";
    static const char* DOCTORES     = "doctores.bin";
    static const char* CITAS        = "citas.bin";
    static const char* HISTORIALES  = "historiales.bin";
}

struct ArchivoHeader {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
    int ultimoId; // si lo necesitas (en citas lo usaste)
};

#endif

#ifndef PACIENTE_HPP
#define PACIENTE_HPP

//CLASE PACIENTE
class Paciente {
private:
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

public:
    // Constructores y destructor
    Paciente();
    Paciente(int idInicial, const char* nom, const char* ape, const char* ced);
    Paciente(const Paciente& otro);
    ~Paciente();

    // Getters (const)
    int getId() const;
    const char* getNombre() const;
    const char* getApellido() const;
    const char* getCedula() const;
    int getEdad() const;
    char getSexo() const;
    const char* getTipoSangre() const;
    const char* getTelefono() const;
    const char* getDireccion() const;
    const char* getEmail() const;
    const char* getAlergias() const;
    const char* getObservaciones() const;
    bool getActivo() const;
    bool getEliminado() const;
    int getCantidadConsultas() const;
    int getPrimerConsultaID() const;
    int getCantidadCitas() const;
    const int* getCitasIDs() const;
    time_t getFechaCreacion() const;
    time_t getFechaModificacion() const;

    // Setters (con validación y actualización de fechaModificacion)
    void setId(int nuevoId);
    void setNombre(const char* nom);
    void setApellido(const char* ape);
    void setCedula(const char* ced);
    void setEdad(int nuevaEdad);
    void setSexo(char s);
    void setTipoSangre(const char* ts);
    void setTelefono(const char* tel);
    void setDireccion(const char* dir);
    void setEmail(const char* mail);
    void setAlergias(const char* a);
    void setObservaciones(const char* o);
    void setActivo(bool a);
    void setEliminado(bool e);

    // Gestión de relaciones
    bool agregarCitaID(int citaID);
    bool eliminarCitaID(int citaID);
    bool tieneCitas() const;

    // Validaciones específicas
    bool validarDatos() const;      // nombre, apellido, cedula, edad en rango, etc.
    bool esMayorDeEdad() const;
    bool cedulaEsValida() const;    // puede delegar en Validaciones

    // Presentación
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;

    // Tamaño estático (para persistencia)
    static size_t obtenerTamano();
};

#include "pacientes/Paciente.hpp"


Paciente::Paciente() : id(0), edad(0), sexo('M'), activo(false), cantidadConsultas(0),
    primerConsultaID(-1), cantidadCitas(0), eliminado(false),
    fechaCreacion(0), fechaModificacion(0) {
    nombre[0] = apellido[0] = cedula[0] = tipoSangre[0] = telefono[0] = direccion[0] = email[0] = '\0';
    alergias[0] = observaciones[0] = '\0';
    for (int i = 0; i < 20; ++i) citasIDs[i] = 0;
}

Paciente::Paciente(int idInicial, const char* nom, const char* ape, const char* ced) : Paciente() {
    id = idInicial;
    setNombre(nom);
    setApellido(ape);
    setCedula(ced);
    fechaCreacion = time(nullptr);
    fechaModificacion = fechaCreacion;
}

Paciente::Paciente(const Paciente& o) { std::memcpy(this, &o, sizeof(Paciente)); }
Paciente::~Paciente() {}

int Paciente::getId() const { return id; }
const char* Paciente::getNombre() const { return nombre; }
const char* Paciente::getApellido() const { return apellido; }
const char* Paciente::getCedula() const { return cedula; }
int Paciente::getEdad() const { return edad; }
char Paciente::getSexo() const { return sexo; }
const char* Paciente::getTipoSangre() const { return tipoSangre; }
const char* Paciente::getTelefono() const { return telefono; }
const char* Paciente::getDireccion() const { return direccion; }
const char* Paciente::getEmail() const { return email; }
const char* Paciente::getAlergias() const { return alergias; }
const char* Paciente::getObservaciones() const { return observaciones; }
bool Paciente::getActivo() const { return activo; }
int Paciente::getCantidadConsultas() const { return cantidadConsultas; }
int Paciente::getPrimerConsultaID() const { return primerConsultaID; }
int Paciente::getCantidadCitas() const { return cantidadCitas; }
const int* Paciente::getCitasIDs() const { return citasIDs; }
bool Paciente::getEliminado() const { return eliminado; }
time_t Paciente::getFechaCreacion() const { return fechaCreacion; }
time_t Paciente::getFechaModificacion() const { return fechaModificacion; }


#include "Paciente.hpp"
#include <cstring>
#include <ctime>
#include <iostream>

// Setter para ID
void Paciente::setId(int nuevoId) {
    if (nuevoId > 0) {
        id = nuevoId;
        fechaModificacion = time(nullptr);
    }
}

// Setter para nombre
void Paciente::setNombre(const char* nom) {
    if (nom != nullptr) {
        std::strncpy(nombre, nom, 49);
        nombre[49] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para apellido
void Paciente::setApellido(const char* ape) {
    if (ape != nullptr) {
        std::strncpy(apellido, ape, 49);
        apellido[49] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para cédula
void Paciente::setCedula(const char* ced) {
    if (ced != nullptr) {
        std::strncpy(cedula, ced, 19);
        cedula[19] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para edad (validación 0–120 años)
void Paciente::setEdad(int nuevaEdad) {
    if (nuevaEdad >= 0 && nuevaEdad <= 120) {
        edad = nuevaEdad;
        fechaModificacion = time(nullptr);
    }
}

// Setter para sexo (solo M/F)
void Paciente::setSexo(char s) {
    if (s == 'M' || s == 'F') {
        sexo = s;
        fechaModificacion = time(nullptr);
    }
}

// Setter para tipo de sangre
void Paciente::setTipoSangre(const char* ts) {
    if (ts != nullptr) {
        std::strncpy(tipoSangre, ts, 4);
        tipoSangre[4] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para teléfono
void Paciente::setTelefono(const char* tel) {
    if (tel != nullptr) {
        std::strncpy(telefono, tel, 14);
        telefono[14] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para dirección
void Paciente::setDireccion(const char* dir) {
    if (dir != nullptr) {
        std::strncpy(direccion, dir, 99);
        direccion[99] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para email (validación mínima con '@')
void Paciente::setEmail(const char* mail) {
    if (mail != nullptr && std::strchr(mail, '@') != nullptr) {
        std::strncpy(email, mail, 49);
        email[49] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para alergias
void Paciente::setAlergias(const char* a) {
    if (a != nullptr) {
        std::strncpy(alergias, a, 499);
        alergias[499] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para observaciones
void Paciente::setObservaciones(const char* o) {
    if (o != nullptr) {
        std::strncpy(observaciones, o, 499);
        observaciones[499] = '\0';
        fechaModificacion = time(nullptr);
    }
}

// Setter para activo
void Paciente::setActivo(bool a) {
    activo = a;
    fechaModificacion = time(nullptr);
}

// Setter para eliminado
void Paciente::setEliminado(bool e) {
    eliminado = e;
    fechaModificacion = time(nullptr);
}
#include "Paciente.hpp"
#include "../utilidades/Validaciones.hpp" // para validar cédula si lo deseas

// ================= RELACIONES =================
bool Paciente::agregarCitaID(int citaID) {
    if (cantidadCitas >= 20) return false; // límite de 20 citas
    citasIDs[cantidadCitas++] = citaID;
    fechaModificacion = time(nullptr);
    return true;
}

bool Paciente::eliminarCitaID(int citaID) {
    bool encontrado = false;
    for (int i = 0; i < cantidadCitas; i++) {
        if (citasIDs[i] == citaID) {
            // mover los siguientes hacia atrás
            for (int j = i; j < cantidadCitas - 1; j++) {
                citasIDs[j] = citasIDs[j + 1];
            }
            citasIDs[cantidadCitas - 1] = 0;
            cantidadCitas--;
            encontrado = true;
            break;
        }
    }
    if (encontrado) fechaModificacion = time(nullptr);
    return encontrado;
}

bool Paciente::tieneCitas() const {
    return cantidadCitas > 0;
}

// ================= VALIDACIONES =================
bool Paciente::validarDatos() const {
    if (std::strlen(nombre) == 0) return false;
    if (std::strlen(apellido) == 0) return false;
    if (std::strlen(cedula) == 0) return false;
    if (edad < 0 || edad > 120) return false;
    if (!(sexo == 'M' || sexo == 'F')) return false;
    if (!Validaciones::validarEmail(email)) return false;
    return true;
}

bool Paciente::esMayorDeEdad() const {
    return edad >= 18;
}

bool Paciente::cedulaEsValida() const {
    return Validaciones::validarCedula(cedula);
}

// ================= PRESENTACIÓN =================
void Paciente::mostrarInformacionBasica() const {
    std::cout << "ID: " << id
              << " | Nombre: " << nombre << " " << apellido
              << " | Cédula: " << cedula << "\n";
}

void Paciente::mostrarInformacionCompleta() const {
    std::cout << "=== Información del Paciente ===\n";
    std::cout << "ID: " << id << "\n";
    std::cout << "Nombre: " << nombre << " " << apellido << "\n";
    std::cout << "Cédula: " << cedula << "\n";
    std::cout << "Edad: " << edad << "\n";
    std::cout << "Sexo: " << sexo << "\n";
    std::cout << "Tipo de sangre: " << tipoSangre << "\n";
    std::cout << "Teléfono: " << telefono << "\n";
    std::cout << "Dirección: " << direccion << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Alergias: " << alergias << "\n";
    std::cout << "Observaciones: " << observaciones << "\n";
    std::cout << "Activo: " << (activo ? "Sí" : "No") << "\n";
    std::cout << "Eliminado: " << (eliminado ? "Sí" : "No") << "\n";
    std::cout << "Cantidad de consultas: " << cantidadConsultas << "\n";
    std::cout << "Cantidad de citas: " << cantidadCitas << "\n";
}

// ================= PERSISTENCIA =================
size_t Paciente::obtenerTamano() {
    return sizeof(Paciente);
}

