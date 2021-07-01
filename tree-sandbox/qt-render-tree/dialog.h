#ifndef DIALOG_H_
#define DIALOG_H_

#include <QDialog>
#include <QGraphicsScene>
#include "tree.h"

namespace Ui
{
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:

    explicit Dialog(QWidget *my_parent = 0);
    ~Dialog();

private:

    void CreateTree();
    void RenderNode(qreal x_pos, qreal y_pos, const Node<int> *node);
    void RenderTree(Tree<int> &tree);
    qreal RenderTreeStep(qreal y_pos, qreal y_delta, qreal x_start, qreal x_end, const Node<int> *node);

    Ui::Dialog *ui;

    QGraphicsScene *scene_;
    QBrush brush_;
    QPen pen_;
    Tree<int> tree_;
};
#endif // DIALOG_H_
