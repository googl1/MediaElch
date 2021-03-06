#include "ConcertModel.h"

#include <QPainter>
#include "globals/Globals.h"
#include "globals/Helper.h"

/**
 * @brief ConcertModel::ConcertModel
 * @param parent
 */
ConcertModel::ConcertModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

/**
 * @brief Adds a concert to the model
 * @param concert Concert to add
 */
void ConcertModel::addConcert(Concert *concert)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_concerts.append(concert);
    endInsertRows();
    connect(concert, SIGNAL(sigChanged(Concert*)), this, SLOT(onConcertChanged(Concert*)), Qt::UniqueConnection);
}

/**
 * @brief Called when a concerts data has changed
 * Emits dataChanged
 * @param concert Concert which has changed
 */
void ConcertModel::onConcertChanged(Concert *concert)
{
    QModelIndex index = createIndex(m_concerts.indexOf(concert), 0);
    emit dataChanged(index, index);
}

/**
 * @brief Get a specific concert
 * @param row Row of the concert
 * @return Concert object
 */
Concert *ConcertModel::concert(int row)
{
    return m_concerts.at(row);
}

/**
 * @brief Returns the rowcount in our model. (=number of movies)
 * @param parent
 * @return Number of rows (=number of movies)
 */
int ConcertModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_concerts.size();
}

/**
 * @brief Get the column count of our model
 * @param parent
 * @return 1
 */
int ConcertModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    // return roleNames().size();
    return 1;
}

/**
 * @brief Accesses items data
 * @param index Index of item
 * @param role Role
 * @return data
 */
QVariant ConcertModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > m_concerts.count())
        return QVariant();

    if (role == Qt::UserRole)
        return index.row();

    Concert *concert = m_concerts[index.row()];
    if (index.column() == 0 && role == Qt::DisplayRole) {
        return Helper::appendArticle(concert->name());
    } else if (index.column() == 0 && role == Qt::ToolTipRole) {
        if (concert->files().size() == 0)
            return QVariant();
        return concert->files().at(0);
    } else if (index.column() == 1 && role == Qt::DisplayRole) {
        return concert->folderName();
    } else if (role == Qt::UserRole+1) {
        return concert->infoLoaded();
    } else if (role == Qt::UserRole+2) {
        return concert->hasChanged();
    } else if (role == Qt::UserRole+3) {
        return concert->syncNeeded();
    } else if (role == Qt::ForegroundRole) {
        if (concert->hasChanged())
            return QColor(255, 0, 0);
    } else if (role == Qt::FontRole) {
        if (concert->hasChanged()) {
            QFont font;
            font.setItalic(true);
            return font;
        }
    }
    return QVariant();
}

/**
 * @brief Returns an empty modelindex because no item has a parent
 * @param child Childindex
 * @return Modelindex of the parent item
 */
QModelIndex ConcertModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

/**
 * @brief Returns a modelindex for the given row and column
 * Parent is not used because our concert model uses only one column.
 * @param row Row of the item
 * @param column Column of the item
 * @param parent Parent modelindex
 * @return Index of the item
 */
QModelIndex ConcertModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

/**
 * @brief Clears the current contents
 */
void ConcertModel::clear()
{
    if (m_concerts.size() == 0)
        return;
    beginRemoveRows(QModelIndex(), 0, m_concerts.size()-1);
    foreach (Concert *concert, m_concerts)
        delete concert;
    m_concerts.clear();
    endRemoveRows();
}

/**
 * @brief Returns a list of all concerts
 * @return List of concerts
 */
QList<Concert*> ConcertModel::concerts()
{
    return m_concerts;
}

/**
 * @brief Checks if there are new concerts (concerts where infoLoaded is false)
 * @return True if there are new concerts
 */
bool ConcertModel::hasNewConcerts()
{
    foreach (Concert *concert, m_concerts) {
        if (!concert->infoLoaded())
            return true;
    }
    return false;
}
