#include <iostream>
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <cassert>
#include "utils.h"
#include "tournament.h"
#include "converter.h"
#include "cliquenetworkmanager.h"

using namespace std;

void learn();
void decompose( const string &in);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cout << "Type a number to decompose words or press enter to relearn data." << endl;

    std::string x;

    getline(cin, x);

    if (x.empty()) {
        learn();
    } else {
        decompose(x);
    }

    //return a.exec();
    return 0;
}

void learn() {
    QFile in("words.txt");
    QFile out("u-list.txt");
    QFile histof("u-histo.txt");

    in.open(QIODevice::ReadOnly);
    out.open(QIODevice::WriteOnly | QIODevice::Text);
    histof.open(QIODevice::WriteOnly | QIODevice::Text);

    QList<QByteArray> _database = in.readAll().split('\n');
    QList<QString> database;

    for (const auto &b : _database) {
        if (!b.contains('\'')) {
            for (int x : b) {
                if (x < 'a' || x > 'z') {
                    goto end;
                }
            }
            database.push_back(QString::fromUtf8(b));
            end:
            ;
        }
    }

    cout << database.size() << " words in the database" << endl;

    Tournament tour;
    Converter convert;
    CliqueNetwork nw;

    nw.init(8, 256);
    tour.init(3, 8, 256);
    convert.fillDummyCliques(10000, 8, 256);
    convert.setSubsequenceMaxSize(2);

    /* While (1) */
    for (int i = 0; i < 100000; i++) {
        /* Histogram of cliques : size / number of words with that size */
        QMap<int, int> histo;

        //Run through all words;
        int maxCliques = 1;
        QString maxword = "";
        for (auto word: database) {
            //nw.learnWord(QString::fromUtf8(word));
            //qDebug() << word;

            /* Transform the word in its clique form, automatically learn letters if not already learned */
            const auto &cls = convert.cliques(word);
            /* Check if the number of subsequences belonging to the word is higher than previously recorded */
            if (cls.size() > maxCliques) {
                maxCliques = cls.size();
                // Store the "longest" word
                maxword = convert.word(cls, true, true);
                //qDebug() << maxword << maxCliques;
            }
            /* Update the histogram */
            histo[cls.size()] += 1;
            /* Update the frequency of the subsequences stored */
            tour.convoluate(cls);
        }

        histof.write("\"" + QByteArray::number(convert.count()) + "\": " + debug(histo).toUtf8() + ", \n");
        histof.flush();

        //learn basic cliques (letters)
        if (i == 0)
        {
            QList<Clique> cliques = convert.cliques();

            for (const auto &cl: cliques) {
                tour.learnClique(cl);
                nw.addClique(cl);
                out.write(convert.word(cl).toUtf8() + "\n");
            }
        }

        //10 most common sequences
        auto most = tour.topSequences(10); tour.clearCache();

        QList<Clique> newCliques;

        //Add most common sequences to cliques list
        for (auto &x : most) {
            assert(x.length() == 2);

            /* Dummy cliques are used as disambiguation */
            x.push_back(convert.getRandomClique());

            assert(x.length() == 3);
            qDebug() << convert.word(x);

            auto clique = convert.getDummyClique();
            newCliques.push_back(clique);
            tour.learnClique(clique);
            nw.addClique(clique);

            for (auto &y: tour.getCoords(x)) {
                //Add links network -> tournament
                for (int i = 0; i < clique.size(); i++) {
                    nw.addLink(cl::coord(i, clique[i]), y.first, y.second);
                }
                //Intra tour links
                for (auto &z: tour.getCoords(x)) {
                    if (y.first != z.first) {
                        y.first->addLink(y.second, z.first, z.second);
                    }
                }
            }

            /* Store the association single clique <-> three cliques */
            convert.associate(clique, x);
            out.write(convert.word(clique).toUtf8() + "\n");
        }
        out.flush();

        //convert.list();
        qDebug() << convert.count();
        qDebug() << "Max cliques: " << maxCliques << maxword;
        qDebug() << "Dummy cliques left: " << convert.getDummyCliques().size();

        //Test a clique is fine
        {
            Clique cl = newCliques.first();
            QList<Clique> cls = convert.cliques(cl);

            assert(cls.size() == 3);

            CliqueNetworkManager mng;
            mng.addNetwork(nw);
            for (int i = 0; i < cls.size(); i++) {
                mng.addNetwork(tour.network(i));
            }

            mng.shutdown();
            nw.activateClique(cl);
            mng.iterate();

            QList<Clique> res;
            for (int i = 0; i < cls.size(); i++) {
                res.push_back(tour.network(i).activatedClique());
            }

            if (convert.word(res) == convert.word(cls)) {
                cout << "Test successful: " << convert.word(cl).toStdString() << " to " << convert.word(res,true).toStdString() << endl;
            } else {
                cout << "Test failed: " << convert.word(cl).toStdString() << " to " << convert.word(res,true).toStdString() << endl;
                return;
            }
        }
    }
}

void decompose(const string &n)
{
    Converter convert;

    int x = std::stoi(n);
    if (x < 50) {
        cout << "Please input a number superior to 50" << endl;
        return;
    }

    auto reload = [&convert](int x) {
        QFile in("list.txt");
        in.open(QIODevice::ReadOnly | QIODevice::Text);

        QList<QByteArray> database = in.readAll().split('\n');

        cout << "Last 10 groups to have been learned: " << endl;

        for (int i = 0; i < x && i < database.size(); i++) {
            if (i+10 >= x) {
                cout << "- " << database[i].data() << endl;
            }
            convert.learnWord(database[i]);
        }
    };

    reload(x);

    cout << "Input a word to have it decomposed or !alea or !pseudo or !gen:" << endl;

    std::string s;

    while (getline(cin, s)) {
        if (s.empty()) {
            return ;
        }
        try {
            int x = std::stoi(s);

            if (x >= 50) {
                convert = Converter();
                reload(x);

                cout << "Input a word to have it decomposed:" << endl;
            }

            continue;
        } catch (...) {

        }

        if (s == "!alea") {
            QMap<int, int> histo;

            QFile in("words.txt");
            in.open(QIODevice::ReadOnly);

            QList<QByteArray> _database = in.readAll().split('\n');
            QList<QString> database;

            for (const auto &b : _database) {
                if (!b.contains('\'')) {
                    for (int x : b) {
                        if (x < 'a' || x > 'z') {
                            goto end;
                        }
                    }
                    database.push_back(QString::fromUtf8(b));
                    end:
                    ;
                }
            }

            std::uniform_int_distribution<> f(0,25);

            for (auto &b : database) {
                for (int i = 0; i < b.size(); i++) {
                    b[i] = 'a' + f(e1);
                }
                histo[convert.cliques(b).size()] += 1;
            }

            cout << debug(histo).toStdString() << endl;
        } else if (s == "!pseudo") {
            QMap<int, int> histo;

            QFile in("pwords.txt");
            in.open(QIODevice::ReadOnly);

            QList<QByteArray> _database = in.readAll().split('\n');
            for (const auto &b : _database) {
                histo[convert.cliques(QString::fromUtf8(b)).size()] += 1;
            }

            cout << debug(histo).toStdString() << endl;
        } else if (s == "!gen") {
            cout << convert.gen(4).toStdString() << endl;
        } else {
            QString decompd = convert.decomposeWord(QString::fromStdString(s));
            auto cls = convert.cliques(QString::fromStdString(s));

            cout << decompd.toStdString() << " (" << cls.size() << ")" << endl;
        }
    }
}
