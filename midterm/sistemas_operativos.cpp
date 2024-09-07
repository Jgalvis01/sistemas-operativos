#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
using namespace std;

/*
MLQ: Simulate a system with multiple queues, where each queue has a different priority level. 
For each queue, you may use Round Robin (RR) or First Come, First Served (FCFS) as the scheduling algorithm.

MLFQ: Extend your MLQ implementation by allowing processes to move between queues based on their execution behavior. 
Design your system to support variable time quanta across queues.
*/




class Process {
private:
    string pid;       // ID del proceso
    int arrival;      // Tiempo de llegada
    int burst;        // Tiempo de ráfaga
    int priority;     // Prioridad
    int start;        // Tiempo de inicio  
    int end;          // Tiempo de finalización
    int tat;   // Tiempo de retorno
    int wt;    // Tiempo de espera
    int quantum;    // Quantum
    bool started;     // Indica si el proceso ha comenzado
    bool executed;    // Indica si el proceso ha terminado

public:

    Process() : pid(""), arrival(0), burst(0), priority(0), start(0), end(0), tat(0), wt(0), quantum(0) {}

    Process(const string &pid, int arrival, int burst, int priority, int start = 0, int end = 0, int tat = 0, int wt = 0, int quantum = 0)
        : pid(pid), arrival(arrival), burst(burst), priority(priority), start(0), end(0), tat(0), wt(0), quantum(0) {}

    void display() const {
        cout << pid << " " << arrival << " " << burst << " " << priority << endl;
    }

    string getPid() const { return pid; }
    int getArrival() const { return arrival; }
    int getBurst() const { return burst; }
    int getPriority() const { return priority; }
    int getStart() const { return start; }
    int getEnd() const { return end; }
    int getTat() const { return tat; }
    int getWt() const { return wt; }
    int setStart(int s) { start = s; }
    int setEnd(int e) { end = e; }
    int setBurst(int b) { burst = b; }
    int setTat(int t) { tat = t; }
    int setWt(int w) { wt = w; }
    int setQuantum( int q)  { quantum = q; }
    int getQuantum() const { return quantum; }
    bool hasStarted() const { return started; }
    void setStarted(bool s) { started = s; }
    bool hasExecuted() const { return executed; }
    void setExecuted(bool e) { executed = e; }
    void setP(int p) { priority = p; }


};

class scheduling {
private:
    map<string, Process> processes;  // Mapa que almacena objetos Process

public:
    
    void readData(const string &filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: No se pudo abrir el archivo " << filename << endl;
            return;
        }

        vector<string> pids;
        vector<int> arrivals, bursts, priorities;
        string line;

        // Leer la primera línea para obtener los PIDs
        getline(file, line);
        stringstream ssPids(line);
        string pid;
        while (ssPids >> pid) {
            pids.push_back(pid);
        }

        // Leer la segunda línea para obtener los arrival times
        getline(file, line);
        stringstream ssArrivals(line);
        int arrival;
        while (ssArrivals >> arrival) {
            arrivals.push_back(arrival);
        }

        // Leer la tercera línea para obtener los burst times
        getline(file, line);
        stringstream ssBursts(line);
        int burst;
        while (ssBursts >> burst) {
            bursts.push_back(burst);
        }

        // Leer la cuarta línea para obtener las prioridades
        getline(file, line);
        stringstream ssPriorities(line);
        int priority;
        while (ssPriorities >> priority) {
            priorities.push_back(priority);
        }

        // Crear objetos Process y agregarlos al mapa usando el pid como clave
        for (size_t i = 0; i < pids.size(); ++i) {
            processes[pids[i]] = Process(pids[i], arrivals[i], bursts[i], priorities[i]);
        }

        file.close();
    }

    void displayProcesses() const {
        for (const auto &pair : processes) {
            pair.second.display();
        }

    }


    Process getProcess() {
        int min = 1000;
        Process first;
        for (const auto &pair : processes) {
            if (pair.second.getArrival() < min) {
                min = pair.second.getArrival();
                first = pair.second;
            }

            break;
        }
        return first;
    } 

    pair<string, int> getFirstProcess(int time) {
        // Inicializar con un proceso válido del mapa
        auto it = processes.begin();
        pair<string, int> first = make_pair(it->second.getPid(), it->second.getArrival());
        for (const auto &pair : processes) {
            if (pair.second.getBurst() > 0 && pair.second.getPid() != processes[first.first].getPid()) {  // Solo considerar procesos con burst restante
                if (processes[first.first].getBurst() == 0) {
                    first = make_pair(pair.second.getPid(), pair.second.getBurst());
                }

                if(pair.second.getBurst() == 0) {
                    break;
                }
                if (pair.second.getArrival() <= time) {  // Proceso que ha llegado en o antes del tiempo actual
                    if (processes[first.first].getArrival() <= time) {
                        if(pair.second.getQuantum() > 0 && processes[first.first].getQuantum() > 0) { 
                            
                            if (pair.second.getPriority() < processes[first.first].getPriority()) {
                                if (pair.second.getQuantum() > 0) {
                                    first = make_pair(pair.second.getPid(), pair.second.getBurst());
                                    
                                }
                            }
                            // Si tienen la misma prioridad, comparar por tiempo de llegada
                            else if (pair.second.getPriority() == processes[first.first].getPriority()) {
                                if (pair.second.getArrival() < processes[first.first].getArrival()) {
                                    first = make_pair(pair.second.getPid(), pair.second.getBurst());
                                }
                                // Si tienen la misma prioridad y tiempo de llegada, escoger por PID (orden lexicográfico)
                                else if (pair.second.getArrival() == processes[first.first].getArrival() && pair.first < first.first) {
                                    first = make_pair(pair.second.getPid(), pair.second.getBurst());
                                }
                            }
                        }
                        
                        else if ( pair.second.getQuantum() > 0 && processes[first.first].getQuantum() == 0) {
                            first = make_pair(pair.second.getPid(), pair.second.getBurst());
                        }
                    } else {
                        first = make_pair(pair.second.getPid(), pair.second.getBurst());
                    }
                }
            }
        }

        if(processes[first.first].getQuantum() > 0) {
            processes[first.first].setQuantum(processes[first.first].getQuantum() - 1);
        }

        

        return first;  // Retornar el proceso seleccionado
    }

    void mlfq() {
        int time = 0;
        int total = 0;
        
        // Mapa para almacenar el burst inicial de cada proceso
        map<string, int> initialBursts;

        // Calcular el burst total y almacenar el burst original en el mapa
        for (const auto &pair : processes) {
            total += pair.second.getBurst();
            initialBursts[pair.second.getPid()] = pair.second.getBurst();  // Guardar el burst inicial
        }

        pair<string, int> first;
        pair<string, int> aux;
        vector<int> order;
        int quantum = 4; // Quantum para Round Robin (cola 1)
        
        // Inicialmente todos los procesos tienen prioridad 1 (RR)
        for (auto it = processes.begin(); it != processes.end(); ++it) {
            it->second.setP(1);
            it->second.setQuantum(quantum); // Asignar quantum a los procesos de la cola 1
        }
        
        while (time < total) {
            first = getFirstProcess(time);  // Seleccionar el primer proceso en base al algoritmo

            // Round Robin (RR) - Cola 1
            if (processes[first.first].getPriority() == 1) { 
                if (processes[first.first].getQuantum() > 0) {
                    // Establecer el inicio del proceso si aún no ha empezado
                    if (time == 0 || !processes[first.first].hasStarted()) {
                        processes[first.first].setStart(time);
                        processes[first.first].setStarted(true);
                    }

                    processes[first.first].setBurst(processes[first.first].getBurst() - 1);
                    processes[first.first].setQuantum(processes[first.first].getQuantum() - 1);

                    // Si el proceso termina en esta cola
                    if (processes[first.first].getBurst() == 0) {
                        processes[first.first].setEnd(time + 1);
                        processes[first.first].setTat(processes[first.first].getEnd() - processes[first.first].getArrival());
                        processes[first.first].setWt(processes[first.first].getTat() - initialBursts[first.first]);
                    }
                    // Si el proceso no termina pero completa su quantum o ha sido atendido una vez
                    if (processes[first.first].getQuantum() == 0 || processes[first.first].getBurst() > 0) {
                        // Mover el proceso a la cola de FCFS (cambiar prioridad a 2)
                        processes[first.first].setP(2);
                    }
                }
            }
            // First-Come, First-Served (FCFS) - Cola 2
            else if (processes[first.first].getPriority() == 2) {
                // Establecer el inicio del proceso si aún no ha empezado
                if (!processes[first.first].hasStarted()) {
                    processes[first.first].setStart(time);
                    processes[first.first].setStarted(true);
                }

                processes[first.first].setBurst(processes[first.first].getBurst() - 1);

                // Si el proceso termina en esta cola
                if (processes[first.first].getBurst() == 0) {
                    processes[first.first].setEnd(time + 1);
                    processes[first.first].setTat(processes[first.first].getEnd() - processes[first.first].getArrival());
                    processes[first.first].setWt(processes[first.first].getTat() - initialBursts[first.first]);
                }
            }

            aux = first;
            time += 1;

            // Reiniciar el quantum de los procesos de RR si es necesario
            bool resetQuantum = true;
            for (const auto &pair : processes) {
                if (pair.second.getPriority() == 1 && pair.second.getQuantum() != 0) {
                    resetQuantum = false;
                }
            }

            // Si todos los procesos de la cola de RR han agotado su quantum, reiniciar el quantum
            if (resetQuantum) {
                for (auto it = processes.begin(); it != processes.end(); ++it) {
                    if (it->second.getPriority() == 1) {
                        it->second.setQuantum(quantum);
                    }
                }
            }
        }
    }



    void mlq() {
        int time = 0;
        int total = 0;
        
        // Mapa para almacenar el burst inicial de cada proceso
        map<string, int> initialBursts;

        // Calcular el burst total y almacenar el burst original en el mapa
        for (const auto &pair : processes) {
            total += pair.second.getBurst();
            initialBursts[pair.second.getPid()] = pair.second.getBurst();  // Guardar el burst inicial
        }

        pair<string, int> first;
        pair<string, int> aux;
        vector<int> order;
        int quantum = 4;
        
        for (auto it = processes.begin(); it != processes.end(); ++it) {
            if (it->second.getPriority() == 1) {
                it->second.setQuantum(quantum);
            }
        }
        
        while (time < total) {
            first = getFirstProcess(time);

            // Round Robin (RR)
            if (processes[first.first].getPriority() == 1) { 
                if (processes[first.first].getQuantum() >= 0) {
                    if (time == 0) {
                        processes[first.first].setStart(time);
                        processes[first.first].setStarted(true);
                    } else if (processes[first.first].getPid() != processes[aux.first].getPid()) {
                        if (processes[first.first].hasStarted() == false) {
                            processes[first.first].setStart(time);
                            processes[first.first].setStarted(true);
                        }
                    }
                    processes[first.first].setBurst(processes[first.first].getBurst() - 1);
                    if (processes[first.first].getBurst() == 0) {
                        processes[first.first].setEnd(time + 1);
                        processes[first.first].setTat(processes[first.first].getEnd() - processes[first.first].getArrival());

                        // Calcular el tiempo de espera (waiting time)
                        processes[first.first].setWt(processes[first.first].getTat() - initialBursts[first.first]);
                    }
                }
            }
            // First-Come, First-Served (FCFS)
            else if (processes[first.first].getPriority() == 2) {
                if (time == 0) {
                    processes[first.first].setStart(time);
                    processes[first.first].setStarted(true);
                } else if (processes[first.first].getPid() != processes[aux.first].getPid()) {
                    if (processes[first.first].hasStarted() == false) {
                        processes[first.first].setStart(time);
                        processes[first.first].setStarted(true);
                    }
                }
                processes[first.first].setBurst(processes[first.first].getBurst() - 1);
                if (processes[first.first].getBurst() == 0) {
                    processes[first.first].setEnd(time + 1);
                    processes[first.first].setTat(processes[first.first].getEnd() - processes[first.first].getArrival());

                    // Calcular el tiempo de espera (waiting time)
                    processes[first.first].setWt(processes[first.first].getTat() - initialBursts[first.first]);
                }
            }

            aux = first;
            time += 1;

            bool flag = true;
            for (const auto &pair : processes) {
                if (pair.second.getQuantum() != 0) {
                    flag = false;
                }
            }

            if (flag) {
                for (auto it = processes.begin(); it != processes.end(); ++it) {
                    if (it->second.getPriority() == 1) {
                        it->second.setQuantum(quantum);
                    }
                }
            }
        }

    }

    double calculateAverageWT() const {
        double totalWT = 0;
        int processCount = 0;

        for (const auto &entry : processes) {
            totalWT += entry.second.getWt();
            processCount++;
        }

        if (processCount == 0) return 0; // Evitar división por cero
        return totalWT / processCount;
    }

    pair<string, int> getProcessWithMaxEnd() {
        if (processes.empty()) {
            throw runtime_error("No processes available.");
        }

        auto it = processes.begin();
        // Inicializar con el primer proceso del mapa
        pair<string, int> maxEndProcess = make_pair(it->first, it->second.getEnd());

        for (const auto &pair : processes) {
            const Process &process = pair.second;
            cout << "Evaluating Process PID: " << process.getPid() 
                << ", End Time: " << process.getEnd() << endl;

            if (process.getEnd() > maxEndProcess.second) {
                cout << "Switching to process with higher End Time PID: " << process.getPid() << endl;
                maxEndProcess = make_pair(process.getPid(), process.getEnd());
            }
        }

        return maxEndProcess;  // Retornar el proceso con mayor tiempo de finalización
    }


    void displayAllProcesses() const {
        if (processes.empty()) {
            cout << "No processes available." << endl;
            return;
        }

        cout << "PID\tArrival\tBurst\tPriority\tStart\tEnd\tTAT\tWT" << endl;
        for (const auto &entry : processes) {
            const Process &process = entry.second;
            cout << process.getPid() << "\t"
                << process.getArrival() << "\t"
                << process.getBurst() << "\t"
                << process.getPriority() << "\t"
                << "         "<< process.getStart() << "\t"
                << process.getEnd() << "\t"
                << process.getTat() << "\t"
                << process.getWt() << "\t" << endl;
        }
    }

    
};

int main() {
    scheduling manager;
    scheduling manager2;
    string filename = "procesos.txt";
    string filename2 = "procesos2.txt";
    manager.readData(filename);
    manager2.readData(filename2);
    manager.displayProcesses();
    manager.mlq();
    manager.displayAllProcesses();
    cout << "Average Waiting Time: " << manager.calculateAverageWT() << endl;
    cout << "---------------------------------" << endl;
    manager2.displayProcesses();
    manager2.mlfq();
    manager2.displayAllProcesses();
    cout << "Average Waiting Time: " << manager2.calculateAverageWT() << endl;

    return 0;
}
