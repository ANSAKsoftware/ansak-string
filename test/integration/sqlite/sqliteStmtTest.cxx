///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2017, Arthur N. Klassen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////
//
// 2014.11.02 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// sqliteStmtTest.cxx -- unit tests for the SQLite statement wrapping class
//
///////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>

#include <sqlite_statement.hxx>
#include <sqlite_db.hxx>
#include <file_path.hxx>
#include <temp_file_wrapper.hxx>
#include <runtime_exception.hxx>

#include <iostream>
#include <string>
#include <string.h>
#include <deque>

using namespace ansak;
using namespace ansak::internal;
using std::string;
using std::deque;
using std::vector;
using namespace testing;

namespace {

void testUp(int xTest, int x, bool xNull,
            double yTest, double y, bool yNull,
            const string& zTest, const string& z, bool zNull,
            const vector<char>& bTest, const vector<char>& b, bool bNull)
{
    int bCount = xNull + yNull + zNull + bNull;
    EXPECT_EQ(3, bCount);
    if (!xNull)
    {
        EXPECT_EQ(xTest, x);
    }
    if (!yNull)
    {
        EXPECT_THAT(yTest, DoubleEq(y));
    }
    if (!zNull)
    {
        EXPECT_EQ(zTest, z);
    }
    if (!bNull)
    {
        EXPECT_EQ(bTest.size(), b.size());
        EXPECT_EQ(0, memcmp(&bTest[0], &b[0], b.size()));
    }
}

void simpleSetupUsableDatabase(TempFileWrapper& tempDir)
{
    try
    {
        FilePath useMe(tempDir.child("useMe"));
        SqliteDB fileDB(useMe);
        fileDB.create();

        fileDB.execute("pragma FOREIGN_KEYS = on;");
        fileDB.execute("create table cities(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);");
        fileDB.execute("insert into cities(name) values('Melbourne');");
        fileDB.execute("insert into cities(name) values('Geelong');");
        fileDB.execute("insert into cities(name) values('Sydney');");
        fileDB.execute("insert into cities(name) values('Brisbane');");
        fileDB.execute("insert into cities(name) values('Perth');");
        fileDB.execute("insert into cities(name) values('Adelaide');");

        fileDB.execute("create table teams(id INTEGER PRIMARY KEY AUTOINCREMENT, cityID INTEGER REFERENCES cities(id), name TEXT,"
                        " wins INTEGER, losses INTEGER, draws INTEGER, for INTEGER, against INTEGER, percent REAL, points INTEGER);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(3, 'Sydney Swans', 17, 5, 0, 2126, 1488, 142.9, 68);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'Hawthorn Hawks', 17, 5, 0, 2458, 1746, 140.8, 68);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(2, 'Geelong Cats', 17, 5, 0, 2033, 1787, 113.8, 68);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(5, 'Fremantle Dockers', 16, 6, 0, 2029, 1556, 130.4, 64);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(6, 'Port Adelaide Power', 14, 8, 0, 2180, 1678, 129.9, 56);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'North Melbourne Kangaroos', 14, 8, 0, 2026, 1731, 117, 56);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'Essendon Bombers', 12, 9, 1, 1828, 1719, 106.3, 50);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'Richmond Tigers', 12, 10, 0, 1887, 1784, 105.8, 48);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(5, 'West Coast Eagles', 11, 11, 0, 2045, 1750, 116.9, 44);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(6, 'Adelaide Crows', 11, 11, 0, 2175, 1907, 114.1, 44);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'Collingwood Magpies', 11, 11, 0, 1766, 1876, 94.1, 44);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(4, 'Gold Coast Suns', 10, 12, 0, 1917, 2045, 93.7, 40);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'Carlton Blues', 7, 14, 1, 1891, 2107, 89.8, 30);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'Western Bulldogs', 7, 15, 0, 1784, 2177, 82, 28);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(4, 'Brisbane Lions', 7, 15, 0, 1532, 2212, 69.3, 28);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(3, 'Great West Sydney Giants', 6, 16, 0, 1780, 2320, 76.7, 24);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'Melbourne Demons', 4, 18, 0, 1336, 1954, 68.4, 16);");
        fileDB.execute("insert into teams(cityID, name, wins, losses, draws, for, against, percent, points) "
                       "values(1, 'St. Kilda Saints', 4, 18, 0, 1480, 2436, 60.8, 16);");
    }
    catch (std::exception& problem)
    {
        std::cout << "Encountered problem during DB setup: " << problem.what() << std::endl;
        throw;
    }
}

void testContents(SqliteDB& db)
{
    try
    {
        db.open();

        string query("select t.name, t.wins, t.losses, t.draws, t.percent, t.points "
                       "from cities c, teams t "
                      "where c.id = t.cityID "
                        "and c.id = 1;");
        SqliteDB::Statement stmt(db.prepare(query));
        string name; int wins, losses, draws, points; double percent;
        bool nameIsNull, winsIsNull, lossesIsNull, drawsIsNull, pointsIsNull, percentIsNull;
        stmt->setupRetrieval(0, name, &nameIsNull);
        stmt->setupRetrieval(1, wins, &winsIsNull);
        stmt->setupRetrieval(2, losses, &lossesIsNull);
        stmt->setupRetrieval(3, draws, &drawsIsNull);
        stmt->setupRetrieval(5, points, &pointsIsNull);
        stmt->setupRetrieval(4, percent, &percentIsNull);
        deque<string> names;
        deque<int> winsPer, lossesPer, drawsPer, pointsPer;
        deque<double> percents;
        bool isDone = false;
        (*stmt)(&isDone);
        while (!isDone)
        {
            EXPECT_FALSE(winsIsNull);
            EXPECT_FALSE(lossesIsNull);
            EXPECT_FALSE(drawsIsNull);
            EXPECT_FALSE(pointsIsNull);
            EXPECT_FALSE(nameIsNull);
            EXPECT_FALSE(percentIsNull);

            EXPECT_FALSE(name.empty());
            names.push_back(name);
            winsPer.push_back(wins);
            lossesPer.push_back(losses);
            drawsPer.push_back(draws);
            percents.push_back(percent);
            pointsPer.push_back(points);

            (*stmt)(&isDone);
        }
        EXPECT_EQ(static_cast<size_t>(9), names.size());
        for (size_t i = 0; i < names.size(); ++i)
        {
            if (names[i] == "Hawthorn Hawks")
            {
                EXPECT_EQ(17, winsPer[i]);
                EXPECT_EQ(5, lossesPer[i]);
                EXPECT_EQ(0, drawsPer[i]);
                EXPECT_EQ(68, pointsPer[i]);
                EXPECT_THAT(140.8, DoubleEq(percents[i]));
            }
            else if (names[i] == "North Melbourne Kangaroos")
            {
                EXPECT_EQ(14, winsPer[i]);
                EXPECT_EQ(8, lossesPer[i]);
                EXPECT_EQ(0, drawsPer[i]);
                EXPECT_EQ(56, pointsPer[i]);
                EXPECT_THAT(117.0, DoubleEq(percents[i]));
            }
            else if (names[i] == "Essendon Bombers")
            {
                EXPECT_EQ(12, winsPer[i]);
                EXPECT_EQ(9, lossesPer[i]);
                EXPECT_EQ(1, drawsPer[i]);
                EXPECT_EQ(50, pointsPer[i]);
                EXPECT_THAT(106.3, DoubleEq(percents[i]));
            }
            else if (names[i] == "Richmond Tigers")
            {
                EXPECT_EQ(12, winsPer[i]);
                EXPECT_EQ(10, lossesPer[i]);
                EXPECT_EQ(0, drawsPer[i]);
                EXPECT_EQ(48, pointsPer[i]);
                EXPECT_THAT(105.8, DoubleEq(percents[i]));
            }
            else if (names[i] == "Collingwood Magpies")
            {
                EXPECT_EQ(11, winsPer[i]);
                EXPECT_EQ(11, lossesPer[i]);
                EXPECT_EQ(0, drawsPer[i]);
                EXPECT_EQ(44, pointsPer[i]);
                EXPECT_THAT(94.1, DoubleEq(percents[i]));
            }
            else if (names[i] == "Carlton Blues")
            {
                EXPECT_EQ(7, winsPer[i]);
                EXPECT_EQ(14, lossesPer[i]);
                EXPECT_EQ(1, drawsPer[i]);
                EXPECT_EQ(30, pointsPer[i]);
                EXPECT_THAT(89.8, DoubleEq(percents[i]));
            }
            else if (names[i] == "Western Bulldogs")
            {
                EXPECT_EQ(7, winsPer[i]);
                EXPECT_EQ(15, lossesPer[i]);
                EXPECT_EQ(0, drawsPer[i]);
                EXPECT_EQ(28, pointsPer[i]);
                EXPECT_THAT(82.0, DoubleEq(percents[i]));
            }
            else if (names[i] == "Melbourne Demons")
            {
                EXPECT_EQ(4, winsPer[i]);
                EXPECT_EQ(18, lossesPer[i]);
                EXPECT_EQ(0, drawsPer[i]);
                EXPECT_EQ(16, pointsPer[i]);
                EXPECT_THAT(68.4, DoubleEq(percents[i]));
            }
            else if (names[i] == "St. Kilda Saints")
            {
                EXPECT_EQ(4, winsPer[i]);
                EXPECT_EQ(18, lossesPer[i]);
                EXPECT_EQ(0, drawsPer[i]);
                EXPECT_EQ(16, pointsPer[i]);
                EXPECT_THAT(60.8, DoubleEq(percents[i]));
            }
            else
            {
                EXPECT_TRUE(i > 9); // should never be true, always blow up, if we get here
            }
        }
    }
    catch (std::exception& problem)
    {
        std::cout << "Encountered problem during DB setup: " << problem.what() << std::endl;
        throw;
    }
}

void prepareOnceandBindSetupUsableDatabase(TempFileWrapper& tempDir)
{
    try
    {
        const char* cities[] = { "Melbourne", "Geelong", "Sydney", "Brisbane", "Adelaide", "Perth", 0 };
        FilePath bindToMe(tempDir.child("bindToMe"));
        SqliteDB fileDB(bindToMe);
        fileDB.create();

        fileDB.execute("pragma FOREIGN_KEYS = on;");
        fileDB.execute("create table cities(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, name TEXT NOT NULL);");
        {
            SqliteDB::Statement cityInsert(fileDB.prepare("insert into cities(name) values(@CITY);"));
            int i = 0;
            while (cities[i] != 0)
            {
                cityInsert->bind(0, cities[i++]);
                cityInsert();
                cityInsert->reset();
            }
        }

        fileDB.execute("create table teams(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                                           "cityID INTEGER REFERENCES cities(id) NOT NULL, "
                                           "name TEXT NOT NULL, "
                                           "wins INTEGER NOT NULL, "
                                           "losses INTEGER NOT NULL, "
                                           "draws INTEGER NOT NULL, "
                                           "for INTEGER NOT NULL, "
                                           "against INTEGER NOT NULL, "
                                           "percent REAL NOT NULL, "
                                           "points INTEGER NOT NULL);");

        {
            int cityX = 0, cityID;
            SqliteDB::Statement cityIDQuery(fileDB.prepare("select id from cities where name = @CITY;"));

            cityIDQuery->bind(0, cities[cityX++]);
            cityIDQuery->setupRetrieval(0, cityID);
            cityIDQuery();
            cityIDQuery->reset();

            SqliteDB::Statement teamInsert(fileDB.prepare("insert into teams(cityID, name, wins, losses, "
                                                                            "draws, for, against, percent, points) "
                                                                    "values(@cityID, @name, @wins, @losses, @draws, @for, "
                                                                           "@against, @percent, @points);"));

            {
                const char* teamsMelbourne[] = { "Hawthorn Hawks", "North Melbourne Kangaroos", "Essendon Bombers",
                                                 "Richmond Tigers", "Collingwood Magpies", "Carlton Blues",
                                                 "Western Bulldogs", "Melbourne Demons", "St. Kilda Saints", 0 };
                const int wltfapMel[9][6] = { { 17, 5, 0, 2458, 1746, 68 }, { 14, 8, 0, 2026, 1731, 56 }, {12, 9, 1, 1828, 1719, 50 },
                                              { 12, 10, 0, 1887, 1784, 48 }, { 11, 11, 0, 1766, 1876, 44 }, { 7, 14, 1, 1891, 2107, 30 },
                                              { 7, 15, 0, 1784, 2177, 28 }, { 4, 18, 0, 1336, 1954, 16 }, {4, 18, 0, 1480, 2436, 16 } };
                const double pctMel[9] = { 140.8, 117, 106.3, 105.8, 94.1, 89.8, 82, 68.4, 60.8 };
                for (int i = 0; teamsMelbourne[i] != 0; ++i)
                {
                    teamInsert->bind(0, cityID);
                    teamInsert->bind(1, teamsMelbourne[i]);
                    teamInsert->bind(2, wltfapMel[i][0]);
                    teamInsert->bind(3, wltfapMel[i][1]);
                    teamInsert->bind(4, wltfapMel[i][2]);
                    teamInsert->bind(5, wltfapMel[i][3]);
                    teamInsert->bind(6, wltfapMel[i][4]);
                    teamInsert->bind(7, pctMel[i]);
                    teamInsert->bind(8, wltfapMel[i][5]);

                    teamInsert();
                    teamInsert->reset();
                }
            }

            cityIDQuery->bind(0, cities[cityX++]);
            cityIDQuery->setupRetrieval(0, cityID);
            cityIDQuery();
            cityIDQuery->reset();

            {
                const char* teamGeel = "Geelong Cats";
                const int wltfapGeel[6] = { 17, 5, 0, 2033, 1787, 68 };
                const double pctGeel = 113.8;

                teamInsert->bind(0, cityID);
                teamInsert->bind(1, teamGeel);
                teamInsert->bind(2, wltfapGeel[0]);
                teamInsert->bind(3, wltfapGeel[1]);
                teamInsert->bind(4, wltfapGeel[2]);
                teamInsert->bind(5, wltfapGeel[3]);
                teamInsert->bind(6, wltfapGeel[4]);
                teamInsert->bind(7, pctGeel);
                teamInsert->bind(8, wltfapGeel[5]);

                teamInsert();
                teamInsert->reset();
            }

            cityIDQuery->bind(0, cities[cityX++]);
            cityIDQuery->setupRetrieval(0, cityID);
            cityIDQuery();
            cityIDQuery->reset();

            {
                const char* teamsSydney[2] = { "Sydney Swans", "Great West Sydney Giants" };
                const int wltfapSydney[2][6] = { { 17, 5, 0, 2126, 1488, 68 }, { 6, 16, 0, 1780, 2320, 24 } };
                const double pctSydney[2] = { 142.9, 76.7 };

                for (int i = 0; i < 2; ++i)
                {
                    teamInsert->bind(0, cityID);
                    teamInsert->bind(1, teamsSydney[i]);
                    teamInsert->bind(2, wltfapSydney[i][0]);
                    teamInsert->bind(3, wltfapSydney[i][1]);
                    teamInsert->bind(4, wltfapSydney[i][2]);
                    teamInsert->bind(5, wltfapSydney[i][3]);
                    teamInsert->bind(6, wltfapSydney[i][4]);
                    teamInsert->bind(7, pctSydney[i]);
                    teamInsert->bind(8, wltfapSydney[i][5]);

                    teamInsert();
                    teamInsert->reset();
                }
            }

            cityIDQuery->bind(0, cities[cityX++]);
            cityIDQuery->setupRetrieval(0, cityID);
            cityIDQuery();
            cityIDQuery->reset();

            {
                const char* teamsBrisbane[2] = { "Gold Coast Suns", "Brisbane Lions" };
                const int wltfapBrisbane[2][6] = { { 10, 12, 0, 1917, 2045, 40 }, { 7, 15, 0, 1532, 2212, 28 } };
                const double pctBrisbane[2] = { 93.7, 69.3 };

                for (int i = 0; i < 2; ++i)
                {
                    teamInsert->bind(0, cityID);
                    teamInsert->bind(1, teamsBrisbane[i]);
                    teamInsert->bind(2, wltfapBrisbane[i][0]);
                    teamInsert->bind(3, wltfapBrisbane[i][1]);
                    teamInsert->bind(4, wltfapBrisbane[i][2]);
                    teamInsert->bind(5, wltfapBrisbane[i][3]);
                    teamInsert->bind(6, wltfapBrisbane[i][4]);
                    teamInsert->bind(7, pctBrisbane[i]);
                    teamInsert->bind(8, wltfapBrisbane[i][5]);

                    teamInsert();
                    teamInsert->reset();
                }
            }

            cityIDQuery->bind(0, cities[cityX++]);
            cityIDQuery->setupRetrieval(0, cityID);
            cityIDQuery();
            cityIDQuery->reset();

            {
                const char* teamsAdelaide[2] = { "Adelaide Crows", "Port Adelaide Power" };
                const int wltfapAdelaide[2][6] = { { 17, 5, 0, 2033, 1787, 68 }, { 14, 8, 0, 2180, 1678, 56 } };
                const double pctAdelaide[2] = { 114.1, 129.9 };

                for (int i = 0; i < 2; ++i)
                {
                    teamInsert->bind(0, cityID);
                    teamInsert->bind(1, teamsAdelaide[i]);
                    teamInsert->bind(2, wltfapAdelaide[i][0]);
                    teamInsert->bind(3, wltfapAdelaide[i][1]);
                    teamInsert->bind(4, wltfapAdelaide[i][2]);
                    teamInsert->bind(5, wltfapAdelaide[i][3]);
                    teamInsert->bind(6, wltfapAdelaide[i][4]);
                    teamInsert->bind(7, pctAdelaide[i]);
                    teamInsert->bind(8, wltfapAdelaide[i][5]);

                    teamInsert();
                    teamInsert->reset();
                }
            }

            cityIDQuery->bind(0, cities[cityX++]);
            cityIDQuery->setupRetrieval(0, cityID);
            cityIDQuery();
            cityIDQuery->reset();

            {
                const char* teamsPerth[2] = { "West Coast Eagles", "Fremantle Dockers" };
                const int wltfapPerth[2][6] = { { 11, 11, 0, 2045, 1750, 44 }, { 16, 6, 0, 2029, 1556, 64 }};
                const double pctPerth[2] = { 116.9, 130.4 };

                for (int i = 0; i < 2; ++i)
                {
                    teamInsert->bind(0, cityID);
                    teamInsert->bind(1, teamsPerth[i]);
                    teamInsert->bind(2, wltfapPerth[i][0]);
                    teamInsert->bind(3, wltfapPerth[i][1]);
                    teamInsert->bind(4, wltfapPerth[i][2]);
                    teamInsert->bind(5, wltfapPerth[i][3]);
                    teamInsert->bind(6, wltfapPerth[i][4]);
                    teamInsert->bind(7, pctPerth[i]);
                    teamInsert->bind(8, wltfapPerth[i][5]);

                    teamInsert();
                    teamInsert->reset();
                }
            }
        }
    }
    catch (std::exception& problem)
    {
        std::cout << "Encountered problem during DB setup: " << problem.what() << std::endl;
        throw;
    }
}

}


TEST(SqliteStatementTest, testPrepare)
{
    TempFileWrapper tempDir;
    FilePath useMe(tempDir.child("useMe"));
    SqliteDB fileDB(useMe);
    fileDB.create();

    SqliteStatementPtr stmtP(fileDB.prepareStatement("select count(*) from sqlite_master;"));
    int rows;
    bool done = false;

    stmtP->setupRetrieval(0, rows);
    (*stmtP)(&done);
    EXPECT_EQ(0, rows);
    EXPECT_FALSE(done);
    (*stmtP)(&done);
    EXPECT_TRUE(done);
}

TEST(SqliteStatementTest, testRetrieve)
{
    TempFileWrapper tempDir;
    simpleSetupUsableDatabase(tempDir);
    FilePath useMe(tempDir.child("useMe"));
    SqliteDB fileDB(useMe);
    testContents(fileDB);
}

TEST(SqliteStatementTest, testBind)
{
    TempFileWrapper tempDir;
    prepareOnceandBindSetupUsableDatabase(tempDir);
    FilePath bindToMe(tempDir.child("bindToMe"));
    SqliteDB fileDB(bindToMe);
    testContents(fileDB);
}

TEST(SqliteStatementTest, testNullity)
{
    TempFileWrapper tempDir;
    FilePath useForNullity(tempDir.child("hasNulls"));
    SqliteDB nullity(useForNullity);
    nullity.create();

    nullity.execute("create table nullable(x INTEGER, y REAL, z TEXT, b BLOB );");
    nullity.execute("insert into nullable(x) values(35);");
    nullity.execute("insert into nullable(y) values(3.14159265358979);");
    nullity.execute("insert into nullable(z) values('Now is the time for all good men...')");
    nullity.execute("insert into nullable(b) values(x'0102030405')");

    SqliteDB::Statement selects(nullity.prepare("select x, y, z, b from nullable"));
    {
        int x;
        double y;
        string z;
        vector<char> b;
        vector<char> bVector = { 1,2,3,4,5 };
        bool xNull, yNull, zNull, bNull;
        selects->setupRetrieval(0, x, &xNull);
        selects->setupRetrieval(1, y, &yNull);
        selects->setupRetrieval(2, z, &zNull);
        selects->setupRetrieval(3, b, &bNull);

        selects();
        testUp(35, x, xNull, 3.14159265358979, y, yNull,
               string("Now is the time for all good men..."), z, zNull, bVector, b, bNull);
        selects();
        testUp(35, x, xNull, 3.14159265358979, y, yNull,
               string("Now is the time for all good men..."), z, zNull, bVector, b, bNull);
        selects();
        testUp(35, x, xNull, 3.14159265358979, y, yNull,
               string("Now is the time for all good men..."), z, zNull, bVector, b, bNull);
        selects();
        testUp(35, x, xNull, 3.14159265358979, y, yNull,
               string("Now is the time for all good men..."), z, zNull, bVector, b, bNull);
    }
}

TEST(SqliteStatementTest, testBlob)
{
}
