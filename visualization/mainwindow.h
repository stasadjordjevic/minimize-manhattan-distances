#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <vector>

using namespace std;


int manhattanDistance(const vector<vector<int>>& points, int i, int j);
pair<int,int> extremePoints(const vector<vector<int>>& points, int skipIndex);

enum Step {
    STEP_INITIAL = 0,
    STEP_FIND_MAX,
    STEP_TRY_P1,
    STEP_TRY_P2,
    STEP_RESULT,
    STEP_COUNT
};

class Canvas : public QWidget {
    Q_OBJECT
public:
    vector<vector<int>> points;
    Step step = STEP_INITIAL;

    int p1 = -1, p2 = -1;
    int wp1a = -1, wp1b = -1;
    int wp2a = -1, wp2b = -1;
    int res1 = 0,  res2 = 0;
    int removedHighlight = -1;

    explicit Canvas(QWidget* parent = nullptr);
    void computeUpTo(Step s);

protected:
    QPointF toScreen(int x, int y) const;
    void drawLine(QPainter& p, int ia, int ib, QColor color,
                  Qt::PenStyle style = Qt::SolidLine, int w = 2);
    void drawPoint(QPainter& p, int idx);
    void paintEvent(QPaintEvent*) override;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void nextStep();
    void prevStep();
    void reset();

private:
    void updateUI();

    Canvas* canvas;
    QLabel* infoLabel;
    QPushButton* btnNext;
    QPushButton* btnPrev;
    QPushButton* btnReset;
    Step currentStep = STEP_INITIAL;
};

#endif // MAINWINDOW_H
