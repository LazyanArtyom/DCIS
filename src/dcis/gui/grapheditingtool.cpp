#include <gui/grapheditingtool.h>


namespace dcis::gui {

GraphEditingTool::GraphEditingTool(QWidget* parent)
    : QWidget(parent)
{
    // image editor widget
    imageEditor_ = new gui::ImageEditor();

    // graph view widget
    graph_ = new graph::Graph(false);
    graphScene_ = new gui::GraphScene(graph_);
    connect(this, &GraphEditingTool::sigGraphChanged, graphScene_, &gui::GraphScene::onReload);

    graphView_ = new gui::GraphView(imageEditor_);
    graphView_->setScene(graphScene_);

    // connections
    connect(graphView_, &gui::GraphView::sigNodeSelected, this , [this](const std::string& nodeName, QPointF pos) {

        //txtConsole_->setText(txt);
    });
    connect(graphView_, &gui::GraphView::sigNodeAdded, this , [this](QPointF pos, bool autoNaming) {
        if (!autoNaming)
        {
            showNewNodeDialog(pos);
            return;
        }

        graph_->addNode(graph::Node(graph_->getNextNodeName(), pos));
        emit sigGraphChanged();
    });
    connect(graphView_, &gui::GraphView::sigNodeRemoved, this, [this](const std::string& nodeName) {
        if (graph_->removeNode(nodeName))
        {
            emit sigGraphChanged();
        }
    });
    connect(graphView_, &gui::GraphView::sigNodeIsolated, this, [this](const std::string& nodeName) {
        if (graph_->isolateNode(nodeName))
        {
            emit sigGraphChanged();
        }
    });
    connect(graphView_, &gui::GraphView::sigEdgeRemoved, this, [this](const std::string& uname, const std::string& vname) {
        if (graph_->removeEdge(uname, vname))
        {
            emit sigGraphChanged();
        }
    });
    connect(graphView_, &gui::GraphView::sigEdgeSet, this, [this](const std::string &uname, const std::string &vname) {
        if (graph_->setEdge(uname, vname))
        {
            emit sigGraphChanged();
        }
    });
    connect(graphView_, &gui::GraphView::sigNodeEdited, this, [this](const std::string& nodeName) {
        bool ok;
        QRegularExpression re(QRegularExpression::anchoredPattern(QLatin1String("[a-zA-Z0-9]{1,30}")));

        auto newName = QInputDialog::getText(this, tr("Rename node"), "Name: ", QLineEdit::Normal,
                                              QString::fromStdString(graph_->getNextNodeName()), &ok);
        if (ok)
        {
            static QRegularExpressionMatch match = re.match(newName);
            if (!match.hasMatch())
            {
                QMessageBox::critical(this, tr("Error"),
                                      tr("Node's name contains only alphabetical or numeric characters\n")
                                      + tr("Length of the name mustn't be greater than 30 or smaller than 1"));
                return;
            }
            if (graph_->hasNode(newName.toStdString()))
            {
                QMessageBox::critical(this, tr("Error"), tr("This name has been used by another node"));
            }
            else
            {
                graph_->setNodeName(nodeName, newName.toStdString());
                emit sigGraphChanged();
            }
        }
    });

    // layouts
    QVBoxLayout* vMainLayout = new QVBoxLayout();
    vMainLayout->setContentsMargins(0, 0, 0, 0);
    vMainLayout->setSpacing(0);

    setLayout(vMainLayout);

    vMainLayout->addWidget(imageEditor_);
}

void GraphEditingTool::showNewNodeDialog(QPointF pos)
{
    bool ok;
    QRegularExpression re(QRegularExpression::anchoredPattern(QLatin1String("[a-zA-Z0-9]{1,3}")));

    QString newNodeName = QInputDialog::getText(this, "Add new node", "Name: ", QLineEdit::Normal,
                                                QString::fromStdString(graph_->getNextNodeName()), &ok);
    if (ok)
    {
        static QRegularExpressionMatch match = re.match(newNodeName);
        if (!match.hasMatch())
        {
            QMessageBox::critical(this, "Error", tr("Node's name contains only alphabetical or numeric characters\n")
                                                 +
                                                 tr("Length of the name mustn't be greater than 3 or smaller than 1"));
            return;
        }

        graph::Node newNode(newNodeName.toStdString(), pos);
        bool succeeded = graph_->addNode(newNode);
        if (!succeeded)
        {
            QMessageBox::critical(this, "Error", "This name has been used by another node");
        }
        else
        {
            emit sigGraphChanged();
        }
    }
}

void GraphEditingTool::onUploadImage()
{
    imageEditor_->onUploadImage();
}

void GraphEditingTool::resizeEvent(QResizeEvent*)
{
    imageEditor_->resize(width(), height());
    graphView_->resize(width(), height());
    //graphView_->setSceneRect(graphView_->rect());
}

} // end namespace dcis::gui
