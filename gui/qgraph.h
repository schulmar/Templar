/*
 * Author:	Arvin Schnell <aschnell@suse.de>
 *
 * Licence:	GPL V2
 */

/*
 * Modified by: Zoltan Borok-Nagy <boroknagyz@gmail.com>
 * 2010-03
 */


#ifndef QGRAPH_H
#define QGRAPH_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPicture>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QList>

#include <graphviz/gvc.h>

/**
 * The QGraph widget shows a graph layouted by graphviz in a
 * QGraphicsView/QGraphicsScene.
 */

class QNode;

class QGraph : public QGraphicsView
{
    Q_OBJECT

public:

    QGraph(QWidget *parent = 0);

    //QGraph(const QString& filename, const QString& layoutAlgorithm, QWidget* parent = 0);

    //QGraph(graph_t* graph, QWidget* parent = 0);

    virtual ~QGraph();

    virtual void renderGraph(GVC_t* gvc, graph_t* graph, const QString& layoutAlgorithm);
    virtual void renderGraph(graph_t* graph);
    virtual void layout(const QString& layoutAlgorithm);
    virtual QGraphicsScene* getScene() { return scene; }

//    QColor getColorOfNode(const QString& id) const;
    QNode* getNodeById(const int &id) const;

    void clearGraph();

    graph_t* getGraph() { return theGraph; }

    void colorizeUpToNode(int nodeId);
    void colorNode(QNode *pNode, const QColor &color);

public slots:
    void colorNode(const int &nodeId, const QColor &color);
    void colorAllNode(const QColor& color);
    bool follow(bool on);
    void selectEntry(const int nodeId);

signals:

    void backgroundContextMenuEvent(QContextMenuEvent* event);
    void nodeContextMenuEvent(QContextMenuEvent* event, const QString& name);
    void nodeDoubleClickEvent(QMouseEvent* event, QString name);
    void nodeClicked(QString name);
    void nodeDoubleClicked(int entryId);

private slots:
    void nodeClick(QNode* node);
    //virtual void BuildGraph();

protected:

    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:

    void init();

    void scaleView(qreal scaleFactor);

    QGraphicsScene* scene;
    int activeNode;
    QList<QNode*> nodeList;
    QRectF graphRect;
    bool mFollow;



    QPointF gToQ(const point& p, bool upside_down = true) const;
    QPointF gToQ(const pointf& p, bool upside_down = true) const;

    QString aggetToQString(void* obj, const char* name, const QString& fallback) const;
    QColor aggetToQColor(void* obj, const char* name, const QColor& fallback) const;
    Qt::PenStyle aggetToQPenStyle(void* obj, const char* name, const Qt::PenStyle fallback) const;

    QPainterPath makeShape(node_t* node) const;
    QPolygonF makeShapeHelper(node_t* node) const;

    QPainterPath makeBezier(const bezier& bezier) const;

    void drawLabel(const textlabel_t* textlabel, QPainter* painter) const;

    void drawArrow(const QLineF& line, const QColor& color, QPainter* painter) const;

private:
    graph_t *theGraph;
    GVC_t *theGvc;
};


class QNode : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:

    QNode(const QPainterPath& path, const QPicture& picture, const int id_,
          const QString &text_, node_t *node_, int kind_);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void mousePressEvent( QGraphicsSceneMouseEvent * );

    QString getText() { return text; }
    void    setText(const QString &n) { text = n; }
    int getId() const { return id; }
    node_t*    getNode() { return node; }
    int getKind() const { return kind; }

signals:
    void clicked(QNode *);

private:
    QPicture picture;
    QString text;
    int id;
    int kind;
private:
    node_t *node;
};


class QEdge : public QGraphicsPathItem
{

public:

    QEdge(const QPainterPath& path, const QPicture& picture);

    QRectF boundingRect() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:

    QPicture picture;

};


#endif
