#include "ImageGallery.h"

#include <QDebug>
#include <QMovie>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QScrollBar>
#include "globals/ImagePreviewDialog.h"

ImageGallery::ImageGallery(QWidget *parent) :
    QWidget(parent)
{
    m_imageWidth = 200;
    m_imageHeight = 100;
    m_alignment = Qt::Vertical;
    m_horizontalSpace = 10;
    m_verticalSpace = 10;
    m_showZoomAndResolution = true;
    m_scrollValue = 50;

    QMovie *loadingMovie = new QMovie(":/img/spinner.gif");
    loadingMovie->start();
    m_loadingLabel = new QLabel(this);
    m_loadingLabel->hide();
    m_loadingLabel->setStyleSheet("background-color: rgba(0, 0, 0, 100); border-radius: 3px;");
    m_loadingLabel->setMovie(loadingMovie);
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setFixedSize(size());

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setFixedSize(size());
    m_imagesWidget = new QWidget(m_scrollArea);
    m_scrollArea->setWidget(m_imagesWidget);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setStyleSheet("background-color: transparent;");

    connect(m_scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVerticalScrollBarMoved(int)));
    connect(m_scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onHorizontalScrollBarMoved(int)));
    connect(m_scrollArea->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(onVerticalScrollBarRangeChanged(int,int)));
    connect(m_scrollArea->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(onHorizontalScrollBarRangeChanged(int,int)));

    m_buttonLeft = new QToolButton(this);
    m_buttonLeft->setIcon(QIcon(":/img/imgButtonLeft.png"));
    m_buttonLeft->setStyleSheet("QToolButton { border: none; }");
    m_buttonLeft->setIconSize(QSize(41, 41));
    m_buttonLeft->raise();
    m_buttonLeft->setAutoRepeat(true);
    m_buttonLeft->setVisible(false);
    m_buttonRight = new QToolButton(this);
    m_buttonRight->setIcon(QIcon(":/img/imgButtonRight.png"));
    m_buttonRight->setStyleSheet("QToolButton { border: none; }");
    m_buttonRight->setIconSize(QSize(41, 41));
    m_buttonRight->raise();
    m_buttonRight->setAutoRepeat(true);
    m_buttonRight->setVisible(false);
    m_buttonTop = new QToolButton(this);
    m_buttonTop->setIcon(QIcon(":/img/imgButtonTop.png"));
    m_buttonTop->setStyleSheet("QToolButton { border: none; }");
    m_buttonTop->setIconSize(QSize(41, 41));
    m_buttonTop->raise();
    m_buttonTop->setAutoRepeat(true);
    m_buttonBottom = new QToolButton(this);
    m_buttonBottom->setIcon(QIcon(":/img/imgButtonBottom.png"));
    m_buttonBottom->setStyleSheet("QToolButton { border: none; }");
    m_buttonBottom->setIconSize(QSize(41, 41));
    m_buttonBottom->raise();
    m_buttonBottom->setAutoRepeat(true);

    connect(m_buttonLeft, SIGNAL(clicked()), this, SLOT(onButtonLeft()));
    connect(m_buttonRight, SIGNAL(clicked()), this, SLOT(onButtonRight()));
    connect(m_buttonTop, SIGNAL(clicked()), this, SLOT(onButtonTop()));
    connect(m_buttonBottom, SIGNAL(clicked()), this, SLOT(onButtonBottom()));
}

void ImageGallery::resizeEvent(QResizeEvent *event)
{
    m_loadingLabel->setFixedSize(size());
    if (m_alignment == Qt::Vertical) {
        m_imagesWidget->setFixedWidth(event->size().width());
        setMinimumWidth(m_imageWidth);
        m_buttonTop->move((event->size().width()-m_buttonTop->width())/2, 0);
        m_buttonBottom->move((event->size().width()-m_buttonBottom->width())/2, event->size().height()-m_buttonBottom->width());
    } else {
        m_imagesWidget->setFixedHeight(m_imageHeight);
        setMinimumHeight(m_imageHeight);
        m_buttonLeft->move(0, (event->size().height()-m_buttonLeft->height())/2);
        m_buttonRight->move(event->size().width()-m_buttonRight->width(), (event->size().height()-m_buttonRight->height())/2);
    }
    m_scrollArea->setFixedSize(size());

    positionImages();
    QWidget::resizeEvent(event);
}

void ImageGallery::clear()
{
    foreach (ClosableImage *label, m_imageLabels) {
        label->hide();
        label->deleteLater();
    }
    m_imageLabels.clear();
}

void ImageGallery::setImages(QList<ExtraFanart> images)
{
    clear();
    foreach (ExtraFanart fanart, images) {
        ClosableImage *label = new ClosableImage(m_imagesWidget);
        label->hide();
        label->setShowZoomAndResolution(m_showZoomAndResolution);
        if (m_alignment == Qt::Vertical)
            label->setFixedSize(Qt::Horizontal, m_imageWidth);
        else
            label->setFixedSize(Qt::Vertical, m_imageHeight);
        label->setMyData(fanart.path);
        if (!fanart.image.isNull())
            label->setImage(fanart.image);
        else
            label->setImage(fanart.path);
        connect(label, SIGNAL(sigClose()), this, SLOT(onCloseImage()));
        m_imageLabels.append(label);
    }
    positionImages();
}

void ImageGallery::addImage(const QByteArray &img, const QString &url)
{
    ClosableImage *label = new ClosableImage(m_imagesWidget);
    label->hide();
    label->setShowZoomAndResolution(m_showZoomAndResolution);
    if (m_alignment == Qt::Vertical)
        label->setFixedSize(Qt::Horizontal, m_imageWidth);
    else
        label->setFixedSize(Qt::Vertical, m_imageHeight);
    label->setImage(img);
    label->setMyData(url);
    connect(label, SIGNAL(sigClose()), this, SLOT(onCloseImage()));
    m_imageLabels.append(label);
    positionImages();
    if (m_alignment == Qt::Vertical)
        m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
    else
        m_scrollArea->horizontalScrollBar()->setValue(m_scrollArea->horizontalScrollBar()->maximum());
}

void ImageGallery::positionImages()
{
    int maxHeightInRow = 0;
    int x = (m_alignment == Qt::Horizontal) ? m_buttonLeft->width()-20 : 0;
    int y = (m_alignment == Qt::Vertical) ? m_buttonTop->height()-20 : 0;

    foreach (ClosableImage *label, m_imageLabels) {
        if (m_alignment == Qt::Vertical) {
            if (x > 0 && x+m_imageWidth+m_horizontalSpace > width()) {
                x = 0;
                y += m_verticalSpace+maxHeightInRow;
                maxHeightInRow = 0;
            }
            label->move(x, y);
            label->show();
            maxHeightInRow = qMax(label->height(), maxHeightInRow);
            x += m_horizontalSpace + m_imageWidth;
        } else {
            label->move(x, 0);
            label->show();
            x += m_horizontalSpace + label->width();
        }
    }
    if (m_alignment == Qt::Vertical) {
        m_imagesWidget->setFixedHeight(y+maxHeightInRow+m_buttonBottom->height()-10);
        m_buttonTop->setEnabled(m_scrollArea->verticalScrollBar()->value() != 0);
        m_buttonBottom->setEnabled(m_scrollArea->verticalScrollBar()->value() != m_scrollArea->verticalScrollBar()->maximum());
        onVerticalScrollBarRangeChanged(m_scrollArea->verticalScrollBar()->minimum(), m_scrollArea->verticalScrollBar()->maximum());
    } else {
        m_imagesWidget->setFixedWidth(x + m_buttonRight->width()-30);
        setFixedHeight(m_imageHeight);
        m_buttonLeft->setEnabled(m_scrollArea->horizontalScrollBar()->value() != 0);
        m_buttonRight->setEnabled(m_scrollArea->horizontalScrollBar()->value() != m_scrollArea->horizontalScrollBar()->maximum());
        onHorizontalScrollBarRangeChanged(m_scrollArea->horizontalScrollBar()->minimum(), m_scrollArea->horizontalScrollBar()->maximum());
    }
}

void ImageGallery::onCloseImage()
{
    ClosableImage *label = static_cast<ClosableImage*>(QObject::sender());
    label->hide();
    label->deleteLater();
    m_imageLabels.removeOne(label);
    positionImages();
    if (!label->myData().toString().isEmpty())
        emit sigRemoveImage(label->myData().toString());
    else
        emit sigRemoveImage(label->image());
}

void ImageGallery::setLoading(const bool &loading)
{
    m_loadingLabel->raise();
    m_loadingLabel->setVisible(loading);
}

void ImageGallery::setAlignment(const int &alignment)
{
    m_alignment = alignment;
    positionImages();

    m_buttonLeft->setVisible(m_alignment == Qt::Horizontal);
    m_buttonRight->setVisible(m_alignment == Qt::Horizontal);
    m_buttonTop->setVisible(m_alignment == Qt::Vertical);
    m_buttonBottom->setVisible(m_alignment == Qt::Vertical);
}

void ImageGallery::setShowZoomAndResolution(const bool &show)
{
    m_showZoomAndResolution = show;
    foreach (ClosableImage *label, m_imageLabels)
        label->setShowZoomAndResolution(show);
    positionImages();
}

void ImageGallery::onVerticalScrollBarMoved(const int &value)
{
    if (m_alignment == Qt::Horizontal)
        return;

    m_buttonTop->setEnabled(value != 0);
    m_buttonBottom->setEnabled(value != m_scrollArea->verticalScrollBar()->maximum());
}

void ImageGallery::onHorizontalScrollBarMoved(const int &value)
{
    if (m_alignment == Qt::Vertical)
        return;

    m_buttonLeft->setEnabled(value != 0);
    m_buttonRight->setEnabled(value != m_scrollArea->horizontalScrollBar()->maximum());
}

void ImageGallery::onButtonLeft()
{
    QPropertyAnimation *anim = new QPropertyAnimation(m_scrollArea->horizontalScrollBar(), "value");
    anim->setStartValue(m_scrollArea->horizontalScrollBar()->value());
    anim->setEndValue(m_scrollArea->horizontalScrollBar()->value()-width()+50);
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void ImageGallery::onButtonRight()
{
    QPropertyAnimation *anim = new QPropertyAnimation(m_scrollArea->horizontalScrollBar(), "value");
    anim->setStartValue(m_scrollArea->horizontalScrollBar()->value());
    anim->setEndValue(m_scrollArea->horizontalScrollBar()->value()+width()-50);
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void ImageGallery::onButtonTop()
{
    QPropertyAnimation *anim = new QPropertyAnimation(m_scrollArea->verticalScrollBar(), "value");
    anim->setStartValue(m_scrollArea->verticalScrollBar()->value());
    anim->setEndValue(m_scrollArea->verticalScrollBar()->value()-height()+100);
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void ImageGallery::onButtonBottom()
{
    QPropertyAnimation *anim = new QPropertyAnimation(m_scrollArea->verticalScrollBar(), "value");
    anim->setStartValue(m_scrollArea->verticalScrollBar()->value());
    anim->setEndValue(m_scrollArea->verticalScrollBar()->value()+height()-100);
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void ImageGallery::onHorizontalScrollBarRangeChanged(int min, int max)
{
    m_buttonLeft->setVisible(min != max);
    m_buttonRight->setVisible(min != max);
}

void ImageGallery::onVerticalScrollBarRangeChanged(int min, int max)
{
    m_buttonTop->setVisible(min != max);
    m_buttonBottom->setVisible(min != max);
}
