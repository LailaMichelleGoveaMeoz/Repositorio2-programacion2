#include<iostream>
#include<fstream>
#include <ctime

using namespace std;

#ifndef HOSPITAL_H
#define HOSPITAL_H

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];

    int siguienteIDPaciente;
    int siguienteIDDoctor;
    int siguienteIDCita;
    int siguienteIDConsulta;

    int totalPacientesRegistrados;
    int totalDoctoresRegistrados;
    int totalCitasAgendadas;
    int totalConsultasRealizadas;
};

struct ArchivoHeader {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
};

#endif

#ifndef PACIENTE_H
#define PACIENTE_H


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

#endif

#ifndef HISTORIAL_H
#define HISTORIAL_H

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
};

#endif

#ifndef DOCTOR_H
#define DOCTOR_H

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

#endif

#ifndef CITA_H
#define CITA_H

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

#endif

//Funciones
#include <fstream>
#include "paciente.h"
#include "hospital.h"

#include <fstream>
#include "paciente.h"
#include "hospital.h"

bool inicializarArchivoPacientes() {
    ArchivoHeader header = {0, 1, 0, 1};
    ofstream archivo("pacientes.bin", ios::binary | ios::out);
    if (!archivo.is_open()) return false;
    archivo.write(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
    archivo.close();
    return true;
}

ArchivoHeader leerHeaderPacientes() {
    ArchivoHeader header;
    ifstream archivo("pacientes.bin", ios::binary);
    archivo.seekg(0);
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
    archivo.close();
    return header;
}

long calcularPosicionPaciente(int indice) {
    return sizeof(ArchivoHeader) + (indice * sizeof(Paciente));
}

int buscarIndicePacientePorID(int id) {
    ifstream archivo("pacientes.bin", ios::binary);
    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
    Paciente temp;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Paciente));
        if (temp.id == id && !temp.eliminado) {
            archivo.close();
            return i;
        }
    }
    archivo.close();
    return -1;
}

Paciente leerPacientePorIndice(int indice) {
    Paciente p;
    ifstream archivo("pacientes.bin", ios::binary);
    long posicion = calcularPosicionPaciente(indice);
    archivo.seekg(posicion);
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Paciente));
    archivo.close();
    return p;
}

bool agregarPaciente(Paciente nuevo) {
    ArchivoHeader header = leerHeaderPacientes();
    nuevo.id = header.proximoID++;
    nuevo.fechaCreacion = time(nullptr);
    nuevo.fechaModificacion = nuevo.fechaCreacion;
    nuevo.eliminado = false;

    ofstream archivo("pacientes.bin", ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    archivo.write(reinterpret_cast<char*>(&nuevo), sizeof(Paciente));
    archivo.close();

    header.cantidadRegistros++;
    header.registrosActivos++;
    actualizarHeaderPacientes(header);
    return true;
}

bool actualizarHeaderPacientes(ArchivoHeader header) {
    fstream archivo("pacientes.bin", ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    archivo.seekp(0);
    archivo.write(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));
    archivo.close();
    return true;
}

bool actualizarPaciente(Paciente modificado) {
    int indice = buscarIndicePacientePorID(modificado.id);
    if (indice == -1) return false;

    modificado.fechaModificacion = time(nullptr);
    fstream archivo("pacientes.bin", ios::binary | ios::in | ios::out);
    long posicion = calcularPosicionPaciente(indice);
    archivo.seekp(posicion);
    archivo.write(reinterpret_cast<char*>(&modificado), sizeof(Paciente));
    archivo.close();
    return true;
}

bool eliminarPaciente(int id) {
    int indice = buscarIndicePacientePorID(id);
    if (indice == -1) return false;

    fstream archivo("pacientes.bin", ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;

    long posicion = calcularPosicionPaciente(indice);
    archivo.seekg(posicion);

    Paciente p;
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Paciente));

    p.eliminado = true;
    p.fechaModificacion = time(nullptr);

    archivo.seekp(posicion);
    archivo.write(reinterpret_cast<char*>(&p), sizeof(Paciente));
    archivo.close();

    ArchivoHeader header = leerHeaderPacientes();
    header.registrosActivos--;
    actualizarHeaderPacientes(header);

    return true;
}

void listarTodosPacientes() {
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) {
        cout << "Error al abrir pacientes.bin\n";
        return;
    }

    ArchivoHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(ArchivoHeader));

    cout << left << setw(5) << "ID"
         << setw(15) << "Nombre"
         << setw(15) << "Apellido"
         << setw(15) << "Cédula"
         << setw(6) << "Edad"
         << setw(10) << "Consultas" << "\n";
    cout << string(66, '-') << "\n";

    Paciente p;
    int activos = 0;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Paciente));
        if (!p.eliminado) {
            cout << left << setw(5) << p.id
                 << setw(15) << p.nombre
                 << setw(15) << p.apellido
                 << setw(15) << p.cedula
                 << setw(6) << p.edad
                 << setw(10) << p.cantidadConsultas << "\n";
            activos++;
        }
    }

    cout << "\nTotal de pacientes activos: " << activos << "\n";
    archivo.close();
}

#include "paciente.h"
#include "historial.h"
#include "hospital.h"

bool agregarConsultaAlHistorial(int pacienteID, HistorialMedico nuevaConsulta) {
    // Leer paciente
    int indicePaciente = buscarIndicePorID<Paciente>("pacientes.bin", pacienteID);
    if (indicePaciente == -1) return false;

    Paciente paciente = leerRegistroPorIndice<Paciente>("pacientes.bin", indicePaciente);

    // Leer header de historiales
    ArchivoHeader headerHistorial = leerHeader("historiales.bin");
    nuevaConsulta.id = headerHistorial.proximoID++;
    nuevaConsulta.pacienteID = pacienteID;
    nuevaConsulta.siguienteConsultaID = -1;
    nuevaConsulta.fechaRegistro = time(nullptr);
    nuevaConsulta.eliminado = false;

    // Si es la primera consulta
    if (paciente.cantidadConsultas == 0) {
        paciente.primerConsultaID = nuevaConsulta.id;
    } else {
        // Buscar última consulta
        int actualID = paciente.primerConsultaID;
        HistorialMedico actual;

        while (true) {
            int indice = buscarIndicePorID<HistorialMedico>("historiales.bin", actualID);
            if (indice == -1) break;
            actual = leerRegistroPorIndice<HistorialMedico>("historiales.bin", indice);
            if (actual.siguienteConsultaID == -1) {
                actual.siguienteConsultaID = nuevaConsulta.id;
                actualizarRegistro("historiales.bin", actual);
                break;
            }
            actualID = actual.siguienteConsultaID;
        }
    }

    // Guardar nueva consulta
    agregarRegistro("historiales.bin", nuevaConsulta);

    // Actualizar paciente
    paciente.cantidadConsultas++;
    actualizarRegistro("pacientes.bin", paciente);

    // Actualizar header de historiales
    headerHistorial.cantidadRegistros++;
    headerHistorial.registrosActivos++;
    actualizarHeader("historiales.bin", headerHistorial);

    return true;
}

HistorialMedico* leerHistorialCompleto(int pacienteID, int* cantidad) {
    int indicePaciente = buscarIndicePorID<Paciente>("pacientes.bin", pacienteID);
    if (indicePaciente == -1) {
        *cantidad = 0;
        return nullptr;
    }

    Paciente paciente = leerRegistroPorIndice<Paciente>("pacientes.bin", indicePaciente);
    if (paciente.cantidadConsultas == 0 || paciente.primerConsultaID == -1) {
        *cantidad = 0;
        return nullptr;
    }

    HistorialMedico* historial = new HistorialMedico[paciente.cantidadConsultas];
    int actualID = paciente.primerConsultaID;
    int contador = 0;

    while (actualID != -1 && contador < paciente.cantidadConsultas) {
        int indice = buscarIndicePorID<HistorialMedico>("historiales.bin", actualID);
        if (indice == -1) break;

        HistorialMedico consulta = leerRegistroPorIndice<HistorialMedico>("historiales.bin", indice);
        if (!consulta.eliminado) {
            historial[contador++] = consulta;
        }

        actualID = consulta.siguienteConsultaID;
    }

    *cantidad = contador;
    return historial;
}

int main(){
	
}
