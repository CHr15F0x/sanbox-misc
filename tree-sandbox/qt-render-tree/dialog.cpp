#include "dialog.h"
#include "ui_dialog.h"
#include <QGraphicsSimpleTextItem>
#include <QDebug>

static const qreal kTextYOffset = -50;
static const qreal kNodeRadius = 20;
static const QFont kFont = QFont("Arial", 15);

Dialog::Dialog(QWidget *my_parent) :
    QDialog(my_parent),
    ui(new Ui::Dialog),
    brush_(Qt::green),
    pen_(Qt::black),
    tree_()
{
    ui->setupUi(this);

    scene_ = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene_);

    pen_.setWidth(2);

    CreateTree();

    RenderTree(tree_);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::CreateTree()
{
    Node<int> *a(new Node<int>("a", 1));
    Node<int> *b(new Node<int>("b", 4));
    Node<int> *c(new Node<int>("c", 4));
    Node<int> *d(new Node<int>("d", 3));
    Node<int> *e(new Node<int>("e", 5));
    Node<int> *f(new Node<int>("f", 6));
    Node<int> *g(new Node<int>("g", 7));
    Node<int> *h(new Node<int>("h", 8));
    Node<int> *i(new Node<int>("i", 9));
    Node<int> *j(new Node<int>("j", 10));
    Node<int> *k(new Node<int>("k", 12));
    Node<int> *l(new Node<int>("l", 11));
    Node<int> *m(new Node<int>("m", 13));
    Node<int> *n(new Node<int>("n", 13));
    Node<int> *o(new Node<int>("o", 103));
    Node<int> *p(new Node<int>("p", 102));
    Node<int> *r(new Node<int>("r", 101));
    tree_.AddNode(a);

    tree_.AddNode(b, "a");
    tree_.AddNode(c, "a");
    tree_.AddNode(d, "a");
    tree_.AddNode(e, "c");
    tree_.AddNode(f, "c");
    tree_.AddNode(g, "f");
    tree_.AddNode(h, "f");
    tree_.AddNode(i, "d");
    tree_.AddNode(j, "i");
    tree_.AddNode(k, "j");
    tree_.AddNode(l, "j");
    tree_.AddNode(m, "j");
    tree_.AddNode(n, "j");
    tree_.AddNode(o, "b");
    tree_.AddNode(p, "b");
    tree_.AddNode(r, "b");
}

void Dialog::RenderNode(qreal x_pos, qreal y_pos, const Node<int> *node)
{
    QGraphicsSimpleTextItem *text = scene_->addSimpleText(
        QString(node->Name()) + ' ' + QString::number(node->Value()), kFont);
    text->setY(y_pos + kTextYOffset);
    text->setX(x_pos - text->boundingRect().width() * 0.5);
    scene_->addEllipse(x_pos - kNodeRadius, y_pos - kNodeRadius, kNodeRadius * 2, kNodeRadius * 2,
        pen_, brush_);
}

void Dialog::RenderTree(Tree<int> &tree)
{
    qreal y_delta = static_cast<qreal>(size().height()) / (tree.Depth() + 1.0);
    qreal root_y = - size().height() * 0.5 + y_delta;

    RenderTreeStep(
        root_y,
        y_delta,
        - size().width() * 0.25,
        size().width() * 0.25,
        tree.Root());

    RenderNode(0, root_y, tree.Root());
}

qreal Dialog::RenderTreeStep(qreal y_pos, qreal y_delta, qreal x_start, qreal x_end, const Node<int> *node)
{
    qreal x_pos = (x_start + x_end);

    if (node->Children().size() == 0)
    {
        return x_pos;
    }

    int i = 0;
    qreal x_delta = (x_end - x_start) / node->Children().size();

    for (auto it = node->Children().cbegin(); it != node->Children().cend(); ++it)
    {
        auto last_x = RenderTreeStep(
            y_pos + y_delta,
            y_delta,
            x_start + i * x_delta,
            x_start + (i + 1) * x_delta,
            it->second);

        scene_->addLine(x_pos, y_pos, last_x, y_pos + y_delta, pen_);

        RenderNode(x_start + i * x_delta + x_start + (i + 1) * x_delta, y_pos + y_delta, it->second);

        ++i;
    }

    return x_pos;
}
