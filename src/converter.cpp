#include <cassert>
#include <QList>
#include <QDebug>
#include "utils.h"
#include "converter.h"

Converter::Converter(int clusters, int fanals) : clusters(clusters), fanals(fanals)
{

}

void Converter::setSubsequenceMaxSize(int size)
{
    subSeqSize = size;
}

QList<QList<int>> Converter::cliques() const
{
    //data.values() doesn't contain dummy cliques
    return m_Cliques;
}

QList<QList<int>> Converter::cliques(const QString &word, bool forceDec)// const
{
    for (const QChar &c : word) {
        learnWord(QString(c));
    }

    QMap<int, QList<Clique>> bestScore;
    bestScore[0];//insert element at 0

    auto bs = [&](int x) {
        if (!bestScore.contains(x)) {
            return 1000000;
        } else {
            return bestScore[x].size();
        }
    };

    for (int i = 1; i <= word.size(); i++) {
        for (int j = 0; j < i; j++) {
            if (j == 0 && i == word.size() && forceDec) {
                continue;
            }
            if (data.contains(word.mid(j, i-j)) && bs(i) > bs(j) + 1) {
                bestScore[i] = bestScore[j];
                bestScore[i].push_back(data[word.mid(j, i-j)]);
            }
        }
    }

    return bestScore[word.size()];
}

QList<QList<int>> Converter::cliques(const Clique &cl)
{
    return rawdata.value(cl);
}

void Converter::learnWord(const QString &word)
{
    if (!data.contains(word)) {
        auto clique = getDummyClique();
        data.insert(word, clique);
        rdata.insert(clique, word);
    }
}

void Converter::fillDummyCliques(int n, int c, int l)
{
    while (n > 0) {
        auto clique = randomClique(c, l);
        rdata.insert(clique, "");
        dummyCliques.push_back(clique);
        m_Cliques.push_back(clique);

        n--;
    }
}

Clique Converter::getDummyClique()
{
    Clique clique;

    if (dummyCliques.empty()) {
        clique =  randomClique(clusters,fanals);
        m_Cliques.push_back(clique);
    } else {
        clique = dummyCliques.takeLast();
    }

    usedCliques.push_back(clique);
    return clique;
}

Clique Converter::getRandomClique() const
{
    std::uniform_int_distribution<> f(0,m_Cliques.size()-1);

    return m_Cliques[f(e1)];
}

Clique Converter::getUsedClique() const
{
    std::uniform_int_distribution<> f(0,usedCliques.size()-1);

    return usedCliques[f(e1)];
}


QList<Clique> Converter::getDummyCliques() const
{
    return dummyCliques;
}

QString Converter::decomposeWord(const QString &word)
{
    auto l = cliques(word, true);
    QStringList ret;

    for (auto c : l) {
        ret.push_back(this->word(c));
    }
    return ret.join('|');
}

void Converter::associate(const Clique &cl, const QList<Clique> &cls)
{
    QString w = word(cls);
    data.insert(w, cl);
    rdata.insert(cl, w);
    rawdata.insert(cl, cls);
}

QString Converter::word(const Clique &data) const
{
    return rdata.value(data, "_");
}

QString Converter::word(const QList<Clique> &l, bool sep, bool full) const
{
    QString ret;

    int counter = 0;
    for (const Clique &c : l) {
        if (!full && subSeqSize && counter >= subSeqSize) {
            break;
        }
        if (sep && ret.size() >  0) {
            ret += "|";
        }
        if (rdata.contains(c)) {
            ret += rdata[c];
        } else {
            ret += "_";
        }

        counter ++;
    }

    return ret;
}

QString Converter::gen(int x) const
{
    QList<QString> l = rdata.values();

    std::uniform_int_distribution<> dist(0, l.size()-1);

    QString ret;

    for (int i = 0; i < x; i++) {
        ret += l[dist(e1)];
    }

    return ret;
}

int Converter::count() const
{
    int ret = rdata.count() - dummyCliques.count();
    assert(ret == usedCliques.size());

    return ret;
}

void Converter::list() const
{
    for (QString s : data.keys()) {
        qDebug() << s;
    }
}
