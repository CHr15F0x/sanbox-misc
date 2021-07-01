#include "dialog.h"
#include "ui_dialog.h"
#include <QGraphicsSimpleTextItem>
#include <QDebug>

static const qreal kTextYOffset = -50;
static const qreal kNodeRadius = 20;
static const QFont kFont = QFont("Arial", 15);

static int int_compare(void *a, void *b)
{
    int a_ = *(static_cast<int *>(a));
    int b_ = *(static_cast<int *>(b));

    if (a_ < b_)
    {
        return -1;
    }

    if (a_ == b_)
    {
        return 0;
    }

    return 1;
}

static value_handlers_t g_handlers;
static value_handlers_t *INT = value_handlers_init(&g_handlers, int_compare, NULL, NULL);
static int values[] =
{
    1, 4, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 13, 103, 102, 101
};

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
    RenderTree();
    tree_clear(&tree_);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::CreateTree()
{
    int *value = &values[0];
    node_t *a = node_create(INT, "a", value++);
    node_t *b = node_create(INT, "b", value++);
    node_t *c = node_create(INT, "c", value++);
    node_t *d = node_create(INT, "d", value++);
    node_t *e = node_create(INT, "e", value++);
    node_t *f = node_create(INT, "f", value++);
    node_t *g = node_create(INT, "g", value++);
    node_t *h = node_create(INT, "h", value++);
    node_t *i = node_create(INT, "i", value++);
    node_t *j = node_create(INT, "j", value++);
    node_t *k = node_create(INT, "k", value++);
    node_t *l = node_create(INT, "l", value++);
    node_t *m = node_create(INT, "m", value++);
    node_t *n = node_create(INT, "n", value++);
    node_t *o = node_create(INT, "o", value++);
    node_t *p = node_create(INT, "p", value++);
    node_t *r = node_create(INT, "r", value++);
    tree_add_node(&tree_, a, NULL);
    tree_add_node(&tree_, b, "a");
    tree_add_node(&tree_, c, "a");
    tree_add_node(&tree_, d, "a");
    tree_add_node(&tree_, e, "c");
    tree_add_node(&tree_, f, "c");
    tree_add_node(&tree_, g, "f");
    tree_add_node(&tree_, h, "f");
    tree_add_node(&tree_, i, "d");
    tree_add_node(&tree_, j, "i");
    tree_add_node(&tree_, k, "j");
    tree_add_node(&tree_, l, "j");
    tree_add_node(&tree_, m, "j");
    tree_add_node(&tree_, n, "j");
    tree_add_node(&tree_, o, "b");
    tree_add_node(&tree_, p, "b");
    tree_add_node(&tree_, r, "b");
}

void Dialog::RenderNode(qreal x_pos, qreal y_pos, node_t *node)
{
    QGraphicsSimpleTextItem *text = scene_->addSimpleText(
        QString(node->object_name) + ' ' + QString::number(*reinterpret_cast<int *>(node->value)), kFont);
    text->setY(y_pos + kTextYOffset);
    text->setX(x_pos - text->boundingRect().width() * 0.5);
    scene_->addEllipse(x_pos - kNodeRadius, y_pos - kNodeRadius, kNodeRadius * 2, kNodeRadius * 2,
        pen_, brush_);
}

void Dialog::RenderTree()
{
    qreal y_delta = static_cast<qreal>(size().height()) / (tree_depth(&tree_) + 1.0);
    qreal root_y = - size().height() * 0.5 + y_delta;

    RenderTreeStep(
        root_y,
        y_delta,
        - size().width() * 0.25,
        size().width() * 0.25,
        tree_.root);

    RenderNode(0, root_y, tree_.root);
}

qreal Dialog::RenderTreeStep(qreal y_pos, qreal y_delta, qreal x_start, qreal x_end, node_t *node)
{
    qreal x_pos = (x_start + x_end);

    if (node->num_children == 0)
    {
        return x_pos;
    }

    qreal x_delta = (x_end - x_start) / node->num_children;

    for (int i = 0; i < node->num_children; ++i)
    {
        auto last_x = RenderTreeStep(
            y_pos + y_delta,
            y_delta,
            x_start + i * x_delta,
            x_start + (i + 1) * x_delta,
            node->children[i]);

        scene_->addLine(x_pos, y_pos, last_x, y_pos + y_delta, pen_);

        RenderNode(x_start + i * x_delta + x_start + (i + 1) * x_delta, y_pos + y_delta, node->children[i]);
    }

    return x_pos;
}
