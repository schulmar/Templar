/*
 * Author:	Arvin Schnell <aschnell@suse.de>
 *
 * Licence:	GPL V2
 */

/*
 * Modified by: Zoltan Borok-Nagy <boroknagyz@gmail.com>
 * 2010-03
 */


#include "../gui/qgraph.h"

#include <math.h>

#include <QKeyEvent>
#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>
#include "common.h"
#include "qt_version_switch.h"

const QColor ActiveColor = Qt::darkBlue;
const double ActiveWidth = 3.0;

QGraph::QGraph(QWidget *parent) : QGraphicsView(parent), mFollow(false)
{
    init();
}

/*QGraph::QGraph(const QString& filename, const QString& layoutAlgorithm, QWidget* parent)
    : QGraphicsView(parent)
{
    init();

    renderGraph(filename, layoutAlgorithm);
}


QGraph::QGraph(graph_t* graph, QWidget* parent)
    : QGraphicsView(parent)
{
    init();

    //renderGraph(graph);
}*/


QGraph::~QGraph()
{
}


void
QGraph::init()
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);

    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    setScene(scene);

    activeNode = -1;
}


void
QGraph::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Plus:
        scaleView(1.2);
        break;
    case Qt::Key_Minus:
        scaleView(1.0 / 1.2);
        break;

#if 0
    case Qt::Key_Asterisk:
        rotate(10.0);
        break;
    case Qt::Key_Slash:
        rotate(-10.0);
        break;
#endif

    default:
        QGraphicsView::keyPressEvent(event);
    }
}


void
QGraph::wheelEvent(QWheelEvent* event)
{
    scaleView(pow(2.0, event->delta() / 240.0));
}


void
QGraph::scaleView(qreal scaleFactor)
{
    qreal f = sqrt(QMATRIX_DETERMINANT(matrix()));

    if (scaleFactor * f > 8.0)
        scaleFactor = 8.0 / f;
    if (scaleFactor * f < 0.05)
        scaleFactor = 0.05 / f;

    scale(scaleFactor, scaleFactor);
}


void
QGraph::contextMenuEvent(QContextMenuEvent* event)
{
    QNode* node = dynamic_cast<QNode*>(itemAt(event->pos()));

    if (node)
        emit nodeContextMenuEvent(event, node->getText());
    else
        emit backgroundContextMenuEvent(event);
}


void
QGraph::mouseDoubleClickEvent(QMouseEvent* event)
{
    QNode* node = dynamic_cast<QNode*>(itemAt(event->pos()));

    if (node){
        emit nodeDoubleClickEvent(event, node->getText());
    }
}


QPointF
QGraph::gToQ(const point& p, bool upside_down) const
{
    return upside_down ? QPointF(p.x, graphRect.height() - p.y) : QPointF(p.x, -p.y);
}


QPointF
QGraph::gToQ(const pointf& p, bool upside_down) const
{
    return upside_down ? QPointF(p.x, graphRect.height() - p.y) : QPointF(p.x, -p.y);
}


QString
QGraph::aggetToQString(void* obj, const char* name, const QString& fallback) const
{
    const char* tmp = agget(obj, const_cast<char*>(name));
    if (tmp == NULL || strlen(tmp) == 0)
        return fallback;
    return QString::fromUtf8(tmp);
}


QColor
QGraph::aggetToQColor(void* obj, const char* name, const QColor& fallback) const
{
    const char* tmp = agget(obj, const_cast<char*>(name));
    if (tmp == NULL || strlen(tmp) == 0)
        return fallback;
    return QColor(tmp);
}


Qt::PenStyle
QGraph::aggetToQPenStyle(void* obj, const char* name, const Qt::PenStyle fallback) const
{
    const char* tmp = agget(obj, const_cast<char*>(name));
    if (tmp == NULL || strlen(tmp) == 0)
        return fallback;
    if (strcmp(tmp, "dashed") == 0)
        return Qt::DashLine;
    if (strcmp(tmp, "dotted") == 0)
        return Qt::DotLine;
    return fallback;
}


QPainterPath
QGraph::makeBezier(const bezier& bezier) const
{
    QPainterPath path;
    path.moveTo(gToQ(bezier.list[0]));
    for (int i = 1; i < bezier.size - 1; i += 3)
        path.cubicTo(gToQ(bezier.list[i]), gToQ(bezier.list[i+1]), gToQ(bezier.list[i+2]));
    return path;
}


void
QGraph::drawArrow(const QLineF& line, const QColor& color, QPainter* painter) const
{
    QLineF n(line.normalVector());
    QPointF o(n.dx() / 3.0, n.dy() / 3.0);

    QPolygonF polygon;
    polygon.append(line.p1() + o);
    polygon.append(line.p2());
    polygon.append(line.p1() - o);

    QPen pen(color);
    pen.setWidthF(1.0);
    painter->setPen(pen);

    QBrush brush(color);
    painter->setBrush(brush);

    painter->drawPolygon(polygon);
}


void
QGraph::renderGraph(GVC_t* gvc, graph_t *graph, const QString& layoutAlgorithm)
{
    //graph_t* graph = agread(fp);
    if (graph != NULL)
    {
        if (gvLayout(gvc, graph, layoutAlgorithm.toUtf8().data()) == 0)
        {
            renderGraph(graph);

            gvFreeLayout(gvc, graph);

            theGvc = gvc;
            theGraph = graph;
        }
        else
        {
            qCritical("gvLayout() failed");
        }

        //agclose(graph);
    }
    else
    {
        qCritical("graph is NULL");
    }
}

void
QGraph::layout(const QString &layoutAlgorithm)
{
    renderGraph(theGvc, theGraph, layoutAlgorithm);
}


QPolygonF
QGraph::makeShapeHelper(node_t* node) const
{
    const polygon_t* poly = (polygon_t*) ND_shape_info(node);

    if (poly->peripheries != 1)
    {
        qWarning("unsupported number of peripheries %d", poly->peripheries);
    }

    const int sides = poly->sides;
    const pointf* vertices = poly->vertices;

    QPolygonF polygon;
    for (int side = 0; side < sides; side++)
        polygon.append(gToQ(vertices[side], false));
    return polygon;
}


QPainterPath
QGraph::makeShape(node_t* node) const
{
    QPainterPath path;

    const char* name = ND_shape(node)->name;

    if ((strcmp(name, "rectangle") == 0) ||
            (strcmp(name, "box") == 0) ||
            (strcmp(name, "hexagon") == 0) ||
            (strcmp(name, "polygon") == 0) ||
            (strcmp(name, "diamond") == 0))
    {
        QPolygonF polygon = makeShapeHelper(node);
        polygon.append(polygon[0]);
        path.addPolygon(polygon);
    }
    else if ((strcmp(name, "ellipse") == 0) ||
             (strcmp(name, "circle") == 0))
    {
        QPolygonF polygon = makeShapeHelper(node);
        path.addEllipse(QRectF(polygon[0], polygon[1]));
    }
    else
    {
        qWarning("unsupported shape %s", name);
    }

    return path;
}


void
QGraph::drawLabel(const textlabel_t* textlabel, QPainter* painter) const
{
    painter->setPen(textlabel->fontcolor);

    // Since I always just take the points from graphviz and pass them to Qt
    // as pixel I also have to set the pixel size of the font.
    QFont font(textlabel->fontname, textlabel->fontsize);
    font.setPixelSize(textlabel->fontsize);
    painter->setFont(font);

    QString text(QString::fromUtf8(textlabel->text));
    QFontMetricsF fm(painter->fontMetrics());
    QRectF rect(fm.boundingRect(text));

    int countBreaks = text.count('\n');

    rect.setHeight(rect.height() * (countBreaks + 1) );
    rect.moveCenter(gToQ(textlabel->pos, false));
    painter->drawText(rect.adjusted(-1, -1, +1, +1), Qt::AlignCenter, text);
}


void
QGraph::clearGraph()
{
    nodeList.clear();
    QList<QGraphicsItem*> items(scene->items());
    while (!items.isEmpty())
        delete items.takeFirst();
    scene->clear();
}


void
QGraph::renderGraph(graph_t* graph)
{
    clearGraph();

    if (GD_charset(graph) != 0)
    {
        qWarning("unsupported charset");
    }

    // don't use gToQ here since it adjusts the values
    graphRect = QRectF(GD_bb(graph).LL.x, GD_bb(graph).LL.y, GD_bb(graph).UR.x, GD_bb(graph).UR.y);
    scene->setSceneRect(graphRect.adjusted(-5, -5, +5, +5));

    scene->setBackgroundBrush(aggetToQColor(graph, "bgcolor", Qt::white));

    for (node_t* node = agfstnode(graph); node != NULL; node = agnxtnode(graph, node))
    {
        QPicture picture;
        QPainter painter;

        painter.begin(&picture);
        drawLabel(ND_label(node), &painter);
        painter.end();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
        char *nodeText = agget(node, "label");
        if (!nodeText)
            nodeText = agnameof(node);
#pragma GCC diagnostic push

        QNode* item = new QNode(makeShape(node), picture, atoi(agnameof(node)), nodeText, node, atoi(agget(node,"nodeKind")));

        item->setPos(gToQ(ND_coord(node)));

        if (activeNode == atoi(agnameof(node))){
            QPen pen(ActiveColor);
            pen.setWidth(ActiveWidth);
            item->setPen(pen);
        } else{
            QPen pen(aggetToQColor(node, "color", Qt::black));
            pen.setWidthF(1.0);
            item->setPen(pen);
        }

        QBrush brush(aggetToQColor(node, "fillcolor", Qt::gray));
        item->setBrush(brush);


        QString tooltip = aggetToQString(node, "tooltip", "");
        if (!tooltip.isEmpty())
        {
            tooltip.replace("\\n", "\n");
            item->setToolTip(tooltip);
        }

        scene->addItem(item);
        nodeList.append(item);

        /*if (agget(node, "fillcolor") == agget(node, "active_color") ){
            activeNode = item;
        }*/

        QObject::connect(item, SIGNAL(clicked(QNode*)), this, SLOT(nodeClick(QNode*)) );

        for (edge_t* edge = agfstout(graph, node); edge != NULL; edge = agnxtout(graph, edge))
        {
            const splines* spl = ED_spl(edge);
            if (spl == NULL)
                continue;

            for (int i = 0; i < spl->size; ++i)
            {
                const bezier& bz = spl->list[i];

                QColor color(aggetToQColor(edge, "color", Qt::black));

                QPainterPath path(makeBezier(bz));

                QPicture picture;
                QPainter painter;

                painter.begin(&picture);
                if (bz.sflag)
                    drawArrow(QLineF(gToQ(bz.list[0]), gToQ(bz.sp)), color, &painter);
                if (bz.eflag)
                    drawArrow(QLineF(gToQ(bz.list[bz.size-1]), gToQ(bz.ep)), color, &painter);
                painter.end();

                QEdge* item = new QEdge(path, picture);

                QPen pen(color);
                pen.setStyle(aggetToQPenStyle(edge, "style", Qt::SolidLine));
                pen.setWidthF(1.0);
                item->setPen(pen);

                item->setZValue(-1.0);

                scene->addItem(item);
            }
        }
    }
}
void QGraph::colorizeUpToNode(int nodeId)
{
     for (int i = 0; i < nodeList.size(); ++i){
         QNode *pNode = nodeList.at(i);
         if(pNode->getId() > nodeId)
         {
           colorNode(pNode,common::colors::DEFAULT);
         }
         else
         {
             colorNode(pNode,common::colors::nodeColors[pNode->getKind()]);
         }
     }
}
void
QGraph::colorAllNode(const QColor &color)
{
    QBrush brush(color);
    for (int i = 0; i < nodeList.size(); ++i){
        nodeList[i]->setBrush(brush);
        nodeList[i]->update();
    }
}

void
QGraph::colorNode(const int &name, const QColor& color)
{

    QNode *node = getNodeById(name);
    colorNode(node,color);
    if(mFollow)
        centerOn(node);
}
void QGraph::colorNode(QNode *node, const QColor &color)
{
    QBrush brush(color);
    node->setBrush(brush);
    node->update();

    agset(node->getNode(), "fillcolor", Q_TO_C_STRING(color.name()));
}

/*QColor
QGraph::getColorOfNode(const QString &name) const
{
    QNode *node = getNodeById(name);
    return node->brush().color();
}*/

bool
QGraph::follow(bool on)
{
    bool old = mFollow;
    mFollow = on;
    return old;
}

QNode*
QGraph::getNodeById(const int& id) const
{
    for (int i = 0; i < nodeList.size(); ++i){
        if (nodeList.at(i)->getId() == id){
            return nodeList.at(i);
        }
    }
    return 0;
}

void
QGraph::nodeClick(QNode *node)
{
    QNode *active = getNodeById(activeNode);
    if (active != 0){
        QPen pen(Qt::black);
        pen.setWidthF(1.0);
        active->setPen(pen);
    }

    QPen pen(ActiveColor);
    pen.setWidth(ActiveWidth);
    node->setPen(pen);
    node->update();

    activeNode = node->getId();
    emit nodeClicked(node->getText());
}


QNode::QNode(const QPainterPath& path, const QPicture& picture, const int id_, const QString &text_, node_t *node_, int kind_)
    : QGraphicsPathItem(path),
      picture(picture),
      text(text_),
      id(id_),
      kind(kind_),
      node(node_)
{
}


void
QNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->save();
    QGraphicsPathItem::paint(painter, option, widget);
    painter->restore();

    picture.play(painter);
}

void
QNode::mousePressEvent( QGraphicsSceneMouseEvent * )
{
    emit clicked(this);
}


QEdge::QEdge(const QPainterPath& path, const QPicture& picture)
    : QGraphicsPathItem(path),
      picture(picture)
{
}


QRectF
QEdge::boundingRect() const
{
    return QGraphicsPathItem::boundingRect().united(picture.boundingRect());
}


void
QEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->save();
    QGraphicsPathItem::paint(painter, option, widget);
    painter->restore();

    picture.play(painter);
}
