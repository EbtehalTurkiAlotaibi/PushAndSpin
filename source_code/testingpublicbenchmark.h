#ifndef TESTINGPUBLICBENCHMARK_H
#define TESTINGPUBLICBENCHMARK_H

#include <QDialog>
#include <string>

namespace Ui {
class TestingPublicBenchmark;
}

class TestingPublicBenchmark : public QDialog
{
    Q_OBJECT

public:
    explicit TestingPublicBenchmark(QWidget *parent = 0);
    ~TestingPublicBenchmark();
    std::string mapName;
    std::pair <int,int> agentRange;
    int stepSize;


private slots:
    void on_buttonBox_accepted();

    void on_lineEdit_3_editingFinished();

    void on_lineEdit_3_textEdited(const QString &arg1);

    void on_lineEdit_3_returnPressed();

private:
    Ui::TestingPublicBenchmark *ui;
};

#endif // TESTINGPUBLICBENCHMARK_H
