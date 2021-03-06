#include "TvShowDelegate.h"
#include <QDebug>
#include <QPainter>
#include "globals/Globals.h"
#include "globals/Manager.h"
#include "data/TvShow.h"
#include "data/TvShowModelItem.h"

/**
 * @brief TvShowDelegate::TvShowDelegate
 * @param parent
 */
TvShowDelegate::TvShowDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

/**
 * @brief Styles the types of items in the tv show list (show, season and episode)
 * @param painter
 * @param option
 * @param index
 */
void TvShowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QFont newFont;
    #ifdef Q_OS_MAC
    newFont.setPixelSize(8);
    #else
    newFont.setPixelSize(8);
    #endif
    QString newInd = tr("new");
    int newWidth = 21;
    int newHeight = newFont.pixelSize();

    if (index.model()->data(index, TvShowRoles::Type).toInt() == TypeTvShow ) {
        painter->save();
        QFont font;

        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(QPen(option.palette.highlightedText().color()));
        }
        painter->translate(5, 3);
        QRect rect = option.rect;
        rect.setWidth(option.rect.width()-5);

        int showNameHeight = 16;

        font.setBold(true);
        #ifdef Q_OS_MAC
            font.setPointSize(12);
        #else
            font.setPixelSize(12);
        #endif
        if (index.data(TvShowRoles::HasChanged).toBool()) {
            font.setItalic(true);
            if (option.state & QStyle::State_Selected) {
            } else {
                painter->setPen(QPen(index.data(Qt::ForegroundRole).value<QColor>()));
            }
        }

        painter->setFont(font);
        painter->drawText(rect, Qt::AlignTop, index.data().toString());

        if (option.state & QStyle::State_Selected)
            painter->setPen(QPen(option.palette.highlightedText().color()));
        else
            painter->setPen(QPen(option.palette.text().color()));
        int episodeCount = index.data(TvShowRoles::EpisodeCount).toInt();
        font.setBold(false);
        #ifdef Q_OS_MAC
            font.setPointSize(10);
        #else
            font.setPixelSize(11);
        #endif
        painter->setFont(font);

        int xMove = 0;
        int widthMove = 0;
        if (index.model()->data(index, TvShowRoles::IsNew).toBool()) {
            QRect newRect(option.rect.x(), option.rect.y()+showNameHeight+((option.rect.height()-showNameHeight-newHeight-4)/2)-3, newWidth+4, newHeight+4);
            painter->setPen(QColor(58, 135, 173));
            painter->setBrush(QBrush(QColor(58, 135, 173)));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawRoundedRect(newRect, 4, 4);
            painter->setPen(QColor(255, 255, 255));
            painter->drawText(newRect.x(), newRect.y(), newRect.width(), newRect.height()-2, Qt::AlignCenter | Qt::AlignVCenter, newInd);
            xMove += newRect.width()+4;
            widthMove += -rect.height()+showNameHeight;
        }

        if (index.model()->data(index, TvShowRoles::SyncNeeded).toBool()) {
            newWidth = newHeight;
            QRect newRect(option.rect.x()+xMove, option.rect.y()+showNameHeight+((option.rect.height()-showNameHeight-newHeight-4)/2)-3, newWidth+4, newHeight+4);
            painter->setPen(QColor(248, 148, 6));
            painter->setBrush(QBrush(QColor(248, 148, 6)));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawRoundedRect(newRect, 4, 4);
            painter->drawPixmap(newRect.x()+2, newRect.y()+2, newRect.width()-4, newRect.height()-4,
                                QPixmap(":/img/sync.png").scaled(newRect.width()-4, newRect.height()-4, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            xMove += newRect.width()+4;
        }


        if (option.state & QStyle::State_Selected)
            painter->setPen(QPen(option.palette.highlightedText().color()));
        else
            painter->setPen(QPen(index.data(Qt::ForegroundRole).value<QColor>()));

        painter->drawText(rect.x()+xMove, rect.y()+showNameHeight, rect.width()+widthMove, rect.height()-showNameHeight,
                          Qt::AlignTop, QString(tr("%n Episodes", "", episodeCount)));

        painter->restore();

    } else if (index.model()->data(index, TvShowRoles::Type).toInt() == TypeEpisode) {
        painter->save();

        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(QPen(option.palette.highlightedText().color()));
        }

        QFont font;
        #ifdef Q_OS_MAC
        font.setPointSize(font.pointSize()-2);
        #endif

        if (index.data(TvShowRoles::HasChanged).toBool()) {
            font.setItalic(true);
            if (option.state & QStyle::State_Selected) {
            } else {
                painter->setPen(QPen(index.data(Qt::ForegroundRole).value<QColor>()));
            }
        }
        painter->setFont(font);

        int xMove = 0;
        if (index.model()->data(index, TvShowRoles::IsNew).toBool()) {
            QRect newRect(option.rect.x(), option.rect.y()+((option.rect.height()-newHeight-4)/2), newWidth+4, newHeight+4);
            painter->setPen(QColor(58, 135, 173));
            painter->setBrush(QBrush(QColor(58, 135, 173)));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawRoundedRect(newRect, 4, 4);
            painter->setPen(QColor(255, 255, 255));
            painter->drawText(newRect.x(), newRect.y(), newRect.width(), newRect.height()-2, Qt::AlignCenter | Qt::AlignVCenter, newInd);
            xMove += newRect.width()+4;
        }

        if (index.model()->data(index, TvShowRoles::SyncNeeded).toBool()) {
            newWidth = newHeight;
            QRect newRect(option.rect.x()+xMove, option.rect.y()+((option.rect.height()-newHeight-4)/2), newWidth+4, newHeight+4);
            painter->setPen(QColor(248, 148, 6));
            painter->setBrush(QBrush(QColor(248, 148, 6)));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawRoundedRect(newRect, 4, 4);
            painter->drawPixmap(newRect.x()+2, newRect.y()+2, newRect.width()-4, newRect.height()-4,
                                QPixmap(":/img/sync.png").scaled(newRect.width()-4, newRect.height()-4, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            xMove += newRect.width()+4;
        }

        if (option.state & QStyle::State_Selected)
            painter->setPen(QPen(option.palette.highlightedText().color()));
        else
            painter->setPen(QPen(index.data(Qt::ForegroundRole).value<QColor>()));
        painter->translate(xMove, 3);
        painter->drawText(option.rect, Qt::AlignTop,  index.data().toString());
        painter->restore();
    } else if (index.model()->data(index, TvShowRoles::Type).toInt() == TypeSeason ) {
        painter->save();

        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(QPen(option.palette.highlightedText().color()));
        }
        QFont font;
        #ifdef Q_OS_MAC
        font.setPointSize(font.pointSize()-2);
        #endif
        font.setBold(true);
        painter->setFont(font);

        if (index.model()->data(index, TvShowRoles::IsNew).toBool()) {
            QRect newRect(option.rect.x()+5, option.rect.y()+((option.rect.height()-newHeight-4)/2), newWidth+4, newHeight+4);
            painter->setPen(QColor(58, 135, 173));
            painter->setBrush(QBrush(QColor(58, 135, 173)));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawRoundedRect(newRect, 4, 4);
            painter->setPen(QColor(255, 255, 255));
            painter->drawText(newRect.x(), newRect.y(), newRect.width(), newRect.height()-2, Qt::AlignCenter | Qt::AlignVCenter, newInd);
            if (option.state & QStyle::State_Selected)
                painter->setPen(QPen(option.palette.highlightedText().color()));
            else
                painter->setPen(QPen(index.data(Qt::ForegroundRole).value<QColor>()));

            painter->translate(newRect.width()+9, 3);
            painter->drawText(option.rect, Qt::AlignTop,  index.data().toString());
        } else {
            painter->translate(5, 3);
            painter->drawText(option.rect, Qt::AlignTop, index.data().toString());
        }

        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

/**
 * @brief TvShowDelegate::sizeHint
 * @param option
 * @param index
 * @return
 */
QSize TvShowDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    if (index.data(TvShowRoles::Type) == TypeTvShow )
        size.setHeight(36);
    else
        size.setHeight(size.height()+6);
    return size;
}
