#ifndef TESTUSERINPUT_H
#define TESTUSERINPUT_H

#include <QDialog>
#include <Agent.h>

namespace Ui {
class TestUserInput;
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
class TestUserInput : public QDialog
{
    Q_OBJECT

public:
    explicit TestUserInput(QWidget *parent = 0);
    ~TestUserInput();
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

private slots:
    void on_buttonBox_accepted();

    void on_lineEdit_2_editingFinished();

    void on_buttonBox_rejected();

private:
    Ui::TestUserInput *ui;
};

#endif // TESTUSERINPUT_H
