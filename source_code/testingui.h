#ifndef TESTINGUI_H
#define TESTINGUI_H

#include <QDialog>
#include "vector"
#include "Agent.h"


namespace Ui {
class TestingUI;
}
struct BiNode { int id; int cycles_id; int Agent_start; int Agent_goal; };
struct BiEdge { int from; int to; int cycle; } ;
struct BiData { int moves; double exec_time; float perc; int nodes; int edges;int A;};

struct Bicycle {
    int id;
    int x;
    int y;
    std::vector<BiNode> nodes;
};

class TestingUI : public QDialog
{
    Q_OBJECT


private slots:

    void on_lineEdit_7_editingFinished();

    void on_buttonBox_accepted();


    void on_buttonBox_rejected();

public:
    explicit TestingUI(QWidget *parent = 0);
    ~TestingUI();
    std::vector <std::pair <int, int> >Edges;
    std::vector<int> cycles_id;
    std::vector<Agent> agents ;
    int node_number;
    int agent_number;
    int solver;
    std::vector<Bicycle> cycles;
    std::vector <BiEdge> BiEdges;
    std::vector<BiNode> BiNodes;
    bool random;

private:
    Ui::TestingUI *ui;
};

#endif // TESTINGUI_H
