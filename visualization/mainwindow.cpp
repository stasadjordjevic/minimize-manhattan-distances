#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <climits>
#include <cmath>
#include <algorithm>

int manhattanDistance(const vector<vector<int>>& points, int i, int j) {
    return abs(points[i][0]-points[j][0]) + abs(points[i][1]-points[j][1]);
}

pair<int,int> extremePoints(const vector<vector<int>>& points, int skipIndex) {
    int n = points.size();
    int max_sum=INT_MIN;
    int min_sum=INT_MAX;
    int max_diff=INT_MIN;
    int min_diff=INT_MAX;
    int max_sum_idx=-1;
    int min_sum_idx=-1;
    int max_diff_idx=-1;
    int min_diff_idx=-1;

    for (int i = 0; i < n; i++){
        if (i == skipIndex) continue;
        int s = points[i][0] + points[i][1];
        int d = points[i][0] - points[i][1];
        if (s > max_sum){
            max_sum  = s;
            max_sum_idx  = i;
        }
        if (s < min_sum){
            min_sum  = s;
            min_sum_idx  = i;
        }
        if (d > max_diff){
            max_diff = d;
            max_diff_idx = i;
        }
        if (d < min_diff){
            min_diff = d;
            min_diff_idx = i;
        }
    }
    int r1 = max_sum  - min_sum;
    int r2 = max_diff - min_diff;
    if (r2 > r1)
        return {max_diff_idx, min_diff_idx};
    return {max_sum_idx, min_sum_idx};
}


Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setMinimumSize(480, 420);
}

void Canvas::computeUpTo(Step s) {
    step = s;
    removedHighlight = -1;

    if (s >= STEP_FIND_MAX) {
        auto r = extremePoints(points, -1);
        p1 = r.first;
        p2 = r.second;
    }
    if (s >= STEP_TRY_P1) {
        removedHighlight = p1;
        auto r = extremePoints(points, p1);
        wp1a = r.first;
        wp1b = r.second;
        res1 = manhattanDistance(points, wp1a, wp1b);
    }
    if (s >= STEP_TRY_P2) {
        removedHighlight = p2;
        auto r = extremePoints(points, p2);
        wp2a = r.first;
        wp2b = r.second;
        res2 = manhattanDistance(points, wp2a, wp2b);
    }
    if (s == STEP_RESULT) {
        removedHighlight = (res1 <= res2) ? p1 : p2;
    }
    update();
}

QPointF Canvas::toScreen(int x, int y) const {
    int minX=INT_MAX, maxX=INT_MIN, minY=INT_MAX, maxY=INT_MIN;
    for (auto& p : points) {
        minX=min(minX,p[0]);
        maxX=max(maxX,p[0]);
        minY=min(minY,p[1]);
        maxY=max(maxY,p[1]);
    }
    double mg = 60;
    double W = width()-2*mg;
    double H = height()-2*mg;
    double rx = max(maxX-minX, 1), ry = max(maxY-minY, 1);
    double sc = min(W/rx, H/ry);
    double ox = mg + (W - rx*sc)/2;
    double oy = mg + (H - ry*sc)/2;
    return { ox + (x-minX)*sc, height() - oy - (y-minY)*sc };
}

void Canvas::drawLine(QPainter& p, int ia, int ib, QColor color, Qt::PenStyle style, int w) {
    if (ia < 0 || ib < 0) return;
    QPointF a = toScreen(points[ia][0], points[ia][1]);
    QPointF b = toScreen(points[ib][0], points[ib][1]);
    p.setPen(QPen(QColor(color.red(), color.green(), color.blue(), 40), w*5));
    p.drawLine(a, b);
    p.setPen(QPen(color, w, style));
    p.drawLine(a, b);
}

void Canvas::drawPoint(QPainter& p, int idx) {
    bool ghost = (idx == removedHighlight && step != STEP_FIND_MAX);
    QPointF s = toScreen(points[idx][0], points[idx][1]);

    bool isMaxPair = (idx == p1 || idx == p2) && step == STEP_FIND_MAX;

    QColor fill = ghost ? QColor(50,55,80)
                : isMaxPair ? QColor(255,80,100)
                : QColor(70,160,255);
    QColor border = ghost ? QColor(70,75,100) : fill.lighter(130);

    if (!ghost && (idx==wp1a || idx==wp1b)&& step==STEP_TRY_P1)
        fill = QColor(255,180,50);
    if (!ghost && (idx==wp2a || idx==wp2b) && step==STEP_TRY_P2)
        fill = QColor(255,180,50);
    if (!ghost && step==STEP_RESULT) {
        bool winner = (res1<=res2) ? (idx==wp1a || idx==wp1b)
                                   : (idx==wp2a||idx==wp2b);
        if (winner)
            fill = QColor(60,210,120);
    }

    p.setPen(QPen(border, 2));
    p.setBrush(ghost ? QBrush(fill, Qt::Dense4Pattern) : fill);
    p.drawEllipse(s, 11, 11);

    p.setPen(ghost ? QColor(100,110,140) : Qt::white);
    p.setFont(QFont("Courier New", 8, QFont::Bold));
    p.drawText(QRectF(s.x()-11, s.y()-11, 22, 22), Qt::AlignCenter, QString::number(idx));

    p.setPen(ghost ? QColor(80,85,110) : QColor(180,195,230));
    p.setFont(QFont("Courier New", 8));
    QString lbl = ghost
        ? QString("(%1,%2) ✕").arg(points[idx][0]).arg(points[idx][1])
        : QString("(%1,%2)").arg(points[idx][0]).arg(points[idx][1]);
    p.drawText(s.toPoint() + QPoint(14,4), lbl);
}

void Canvas::paintEvent(QPaintEvent*) {
    QPainter p(this);

    //drawing grid
    p.setPen(QPen(QColor(30,34,54), 1));
    for (int x=0; x<width();  x+=36)
        p.drawLine(x, 0, x, height());
    for (int y=0; y<height(); y+=36)
        p.drawLine(0, y, width(), y);

    if (points.empty()) return;

    if (step == STEP_FIND_MAX && p1>=0 && p2>=0) {
        drawLine(p, p1, p2, QColor(255,80,100));
        QPointF a = toScreen(points[p1][0],  points[p1][1]);
        QPointF b = toScreen(points[p2][0],  points[p2][1]);
        QPointF mid((a.x()+b.x())/2-10, (a.y()+b.y())/2-12);
        int d = manhattanDistance(points, p1, p2);
        p.setPen(QColor(255,130,150));
        p.setFont(QFont("Courier New", 9, QFont::Bold));
        p.drawText(mid.toPoint(), QString("d=%1").arg(d));
    }
    if (step == STEP_TRY_P1 && wp1a>=0 && wp1b>=0) {
        drawLine(p, wp1a, wp1b, QColor(255,180,50));
        QPointF a = toScreen(points[wp1a][0], points[wp1a][1]);
        QPointF b = toScreen(points[wp1b][0], points[wp1b][1]);
        QPointF mid((a.x()+b.x())/2-10, (a.y()+b.y())/2-12);
        p.setPen(QColor(255,200,80));
        p.setFont(QFont("Courier New", 9, QFont::Bold));
        p.drawText(mid.toPoint(), QString("d=%1").arg(res1));
    }
    if (step == STEP_TRY_P2 && wp2a>=0 && wp2b>=0) {
        drawLine(p, wp1a, wp1b, QColor(80,100,140), Qt::DashLine);
        drawLine(p, wp2a, wp2b, QColor(255,180,50));
        QPointF a = toScreen(points[wp2a][0], points[wp2a][1]);
        QPointF b = toScreen(points[wp2b][0], points[wp2b][1]);
        QPointF mid((a.x()+b.x())/2-10, (a.y()+b.y())/2-12);
        p.setPen(QColor(255,200,80));
        p.setFont(QFont("Courier New", 9, QFont::Bold));
        p.drawText(mid.toPoint(), QString("d=%1").arg(res2));
    }
    if (step == STEP_RESULT) {
        bool useP1 = (res1 <= res2);
        int ra = useP1 ? wp1a : wp2a;
        int rb = useP1 ? wp1b : wp2b;
        int rd = useP1 ? res1 : res2;
        drawLine(p, ra, rb, QColor(60,210,120), Qt::SolidLine, 3);
        QPointF a = toScreen(points[ra][0], points[ra][1]);
        QPointF b = toScreen(points[rb][0], points[rb][1]);
        QPointF mid((a.x()+b.x())/2-10, (a.y()+b.y())/2-12);
        p.setPen(QColor(100,240,160));
        p.setFont(QFont("Courier New", 10, QFont::Bold));
        p.drawText(mid.toPoint(), QString("d=%1").arg(rd));
    }

    for (int i = 0; i < (int)points.size(); i++)
        drawPoint(p, i);
}


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("minimumDistance — Algorithm Visualization");
    resize(780, 560);
    setStyleSheet(
        "QMainWindow,QWidget {background:#0e1018;color:#c8d0f0;}"
        "QPushButton {background:#1e2240;color:#c8d0f0;border:1px solid #3a3e60;"
        "padding:6px 18px;font:10px 'Courier New';border-radius:4px;}"
        "QPushButton:hover {background:#2a3060;border-color:#5060a0;}"
        "QPushButton:disabled {color:#3a3e60;border-color:#1e2240;}");

    auto* central = new QWidget;
    auto* vlay = new QVBoxLayout(central);
    vlay->setContentsMargins(10,10,10,10);
    vlay->setSpacing(8);
    setCentralWidget(central);

    canvas = new Canvas;
    vlay->addWidget(canvas, 1);

    infoLabel=new QLabel;
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet(
        "font:11px 'Courier New';color:#8890c0;"
        "background:#12141e;border:1px solid #1e2240;"
        "padding:6px;border-radius:4px;");
    infoLabel->setWordWrap(true);
    vlay->addWidget(infoLabel);

    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(8);
    btnPrev= new QPushButton("Prev");
    btnNext= new QPushButton("Next");
    btnReset= new QPushButton("Reset");
    btnRow->addStretch();
    btnRow->addWidget(btnPrev);
    btnRow->addWidget(btnNext);
    btnRow->addWidget(btnReset);
    btnRow->addStretch();
    vlay->addLayout(btnRow);

    connect(btnNext,&QPushButton::clicked,this,&MainWindow::nextStep);
    connect(btnPrev,&QPushButton::clicked,this,&MainWindow::prevStep);
    connect(btnReset,&QPushButton::clicked,this,&MainWindow::reset);

    //canvas->points = {{3,10},{5,15},{10,2},{4,4}};
    canvas->points = {{1,4}, {3,11}, {6,1}, {8,9}, {12,5},
        {15,13}, {10,7}, {4,15}, {13,2}, {7,30}};
    currentStep = STEP_INITIAL;
    canvas->computeUpTo(STEP_INITIAL);
    updateUI();
}

void MainWindow::nextStep() {
    if (currentStep < STEP_RESULT) {
        currentStep = (Step)(currentStep + 1);
        canvas->computeUpTo(currentStep);
        updateUI();
    }
}

void MainWindow::prevStep() {
    if (currentStep > STEP_INITIAL) {
        currentStep = (Step)(currentStep - 1);
        canvas->computeUpTo(currentStep);
        updateUI();
    }
}

void MainWindow::reset() {
    currentStep = STEP_INITIAL;
    canvas->computeUpTo(STEP_INITIAL);
    updateUI();
}

void MainWindow::updateUI() {
    btnPrev->setEnabled(currentStep > STEP_INITIAL);
    btnNext->setEnabled(currentStep < STEP_RESULT);

    QString msg;
    switch (currentStep) {
    case STEP_INITIAL:
        msg = "Step 1/5 — Polazne tacke. Svaka tacka prikazana sa koordinatama.";
        break;
    case STEP_FIND_MAX:
        msg = QString("Step 2/5 — extremePoints(skip=-1): Maksimalan par = (%1, %2),  d = %3")
            .arg(canvas->p1).arg(canvas->p2)
            .arg(manhattanDistance(canvas->points, canvas->p1, canvas->p2));
        break;
    case STEP_TRY_P1:
        msg = QString("Step 3/5 — Uklanjamo tacku %1. Novi max par = (%2, %3),  d = %4")
            .arg(canvas->p1).arg(canvas->wp1a).arg(canvas->wp1b).arg(canvas->res1);
        break;
    case STEP_TRY_P2:
        msg = QString("Step 4/5 — Uklanjamo tacku %1. Novi max par = (%2, %3),  d = %4  |  res1=%5  res2=%6")
            .arg(canvas->p2).arg(canvas->wp2a).arg(canvas->wp2b).arg(canvas->res2)
            .arg(canvas->res1).arg(canvas->res2);
        break;
    case STEP_RESULT: {
        bool useP1 = (canvas->res1 <= canvas->res2);
        int  ans   = min(canvas->res1, canvas->res2);
        msg = QString("Step 5/5 — Rezultat: min(%1, %2) = %3  ->  uklanjamo tacku %4  OK")
            .arg(canvas->res1).arg(canvas->res2).arg(ans)
            .arg(useP1 ? canvas->p1 : canvas->p2);
        break;
    }
    default: break;
    }
    infoLabel->setText(msg);
}
