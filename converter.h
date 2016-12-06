#ifndef CONVERTER_H
#define CONVERTER_H

#include <QList>
#include <QHash>

#include "cliquenetwork.h"

/* Converts text <-> cliques/index */

class Converter
{
public:
    Converter();

    void setSubsequenceMaxSize(int size);

    QList<QList<int>> cliques() const;
    /* Decompose word into cliques, learn individual letters if not in db */
    QList<QList<int>> cliques (const QString &word, bool forceDec = false); //const;
    QList<QList<int>> cliques (const Clique &cl); //const;
    void associate(const Clique &cl, const QList<Clique> &cls);
    QString word (const Clique &data) const;
    QString word (const QList<Clique> &data, bool sep = false, bool full = false) const;
    int count () const;
    void list() const;
    QString gen(int x=4) const;
    QString decomposeWord(const QString &word);
    void learnWord(const QString &word);

    /* When wanting to store/get a "dummy" clique */
    void fillDummyCliques(int n, int c, int l);
    Clique getDummyClique();
    Clique getRandomClique() const;
    QList<Clique> getDummyCliques() const;
private:
    QHash<QString, QList<int>> data;
    QHash<QList<int>, QString> rdata;
    QHash<QList<int>, QList<Clique>> rawdata;

    QList<Clique> dummyCliques;
    QList<Clique> m_Cliques;
    int subSeqSize = 0;
};

#endif // CONVERTER_H
