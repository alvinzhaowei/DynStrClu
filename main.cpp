#include "MyLib/MyTimer.h"
#include "MyLib/ParaReader.h"
#include "graph/Graph.h"
#include <iostream>
#include <set>
#include <stdio.h>
#include <random>

struct Param {
    char graph_file[200];
    char update_file[200];
    double rho;
};

struct Param parseArgs(int nargs, char **args) {
    Param rtn;
    int cnt = 1;
    bool failed = false;
    char *arg;
    int i;
    char para[10];
    char graph_file[200] = "./test_data/Condmat.bin";
    char update_file[200] = "./test_data/10x_Uniform_5.txt";
    double rho = 0.01;

    printf("The input parameters are:\n\n");
    while (cnt < nargs && !failed) {
        arg = args[cnt++];
        if (cnt == nargs) {
            failed = true;
            break;
        }
        i = getNextChar(arg);
        if (arg[i] != '-') {
            failed = true;
            break;
        }
        getNextWord(arg + i + 1, para);
        printf("%s\t", para);
        arg = args[cnt++];
        if (strcmp(para, "graph") == 0) {
            getNextWord(arg, graph_file);
            printf("%s\n", graph_file);
        } else if (strcmp(para, "rho") == 0) {
            rho = atof(arg);
            if (rho < 0 || rho > 1) {
                failed = true;
                break;
            }
            printf("rho : %lf\n", rho);
        } else if (strcmp(para, "update") == 0) {
            getNextWord(arg, update_file);
            printf("%s\n", update_file);
        } else {
            failed = true;
            printf("Unknown option -%s!\n\n", para);
        }
    }

    /*****************************************************************************/
    strcpy(rtn.graph_file, graph_file);
    strcpy(rtn.update_file, update_file);
    rtn.rho = rho;
    return rtn;
}

void usage() {
    printf("Usage:\n");
    printf("dynstrclu -graph [graph file] -update [update file] "
           "-rho [\\rho]\n");
}

int generateRandomInt(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);

    return dis(gen);
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        usage();
        return 0;
    }
    printf("Start to parse the arguments\n");

    Param para = parseArgs(argc, argv);

    printf("Arguments parsed.\n");

    FILE *f = fopen(para.graph_file, "rb");

    if (f == NULL) {
        printf("graph file not found.\n");
        exit(1);
    }

    unsigned int n, m;
    fread(&n, 1, sizeof(int), f);
    fread(&m, 1, sizeof(int), f);
    int *edges = (int *) malloc(sizeof(int) * m);
    fread(edges, m, sizeof(int), f);
    fclose(f);
    f = fopen(para.update_file, "r");
    vector<pair<int, pair<int, int>>> updates;
    updates.reserve(9 * (m / 2));
    int a = 0, b = 0, c = 0;
    while (fscanf(f, "%d%d%d", &a, &b, &c) != EOF) {
        updates.emplace_back(make_pair(a, make_pair(b, c)));
    }
    fclose(f);

    MyVector<dynscan::Vertex *> _vList;
    _vList.reserve(n);

    for (int i = 0; i < n; i++) {
        dynscan::Vertex *newVertex = new dynscan::Vertex(i + 1);
        _vList.push_back(newVertex);
    }
    Graph graph(_vList, para.rho);
    printf("Graph generation finished with %d vertices and %d edges.\n", n,
           m / 2);

    _vList.release_space();
    double start = getCurrentTime();
    double end = 0;
    double output_gap = 0.1;
    double next_output_percentage = output_gap;
    int next_output_position = (m / 2) * next_output_percentage;
    for (int i = 0, j = 0; i < m; i += 2, ++j) {
        graph.insertEdge(edges[i], edges[i + 1]);
        if (j == next_output_position - 1) {
            end = getCurrentTime();
            printf("Total time used after inserting %.2lf m edges: *%.9lf*\t"
                   "Average time in processing one insertion: *%.9lf*\n",
                   next_output_percentage, end - start,
                   (end - start) / (double) (j + 1));
            next_output_percentage += output_gap;
            next_output_position = (m / 2) * next_output_percentage;
        }
    }

    printf("---------------------------------------------------------------------\n");
    next_output_position = (int) (0.1 * m / 2);
    double total_query = 0, total_update = 0, query_times = 0;
    int next_query_position = 20;
    for (long long i = 0, usize = updates.size(); i < usize; ++i) {
        start = getCurrentTime();
        if (updates[i].first == 1) {
            graph.insertEdge(updates[i].second.first, updates[i].second.second);
        } else {
            graph.removeEdge(updates[i].second.first, updates[i].second.second);
        }
        end = getCurrentTime();
        total_update += end - start;
        if (i == next_output_position - 1) {

            next_output_percentage = next_output_position / (double) (m / 2);
            printf("After updating %.2lf m edges, the average time for each update: *%.9lf*\n",
                   next_output_percentage,
                   total_update / (double) (i + 1));

            next_output_position += (int) (0.1 * m / 2);
        }
        if (i == next_query_position - 1) {
            double q_time = 0;
            double esp = generateRandomInt(100, 1000) / 1000.0;
            int mu = generateRandomInt(1, int(2 * m / n));
            q_time = graph.query(esp, mu);
            total_query += q_time;
            query_times++;
            next_query_position += next_query_position;
        }
    }

    printf("---------------------------------------------------------------------\n");
    printf("Average query time: *%.9lf*\t",
           total_query / query_times);
}
