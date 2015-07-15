/*jshint esnext: true */
/*global describe, beforeEach, it, expect, afterEach*/

////////////////////////////////////////////////////////////////////////////////
/// @brief Spec for the AQL FOR x IN GRAPH name statement
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Michael Hackstein
/// @author Copyright 2015, ArangoDB GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

(function() {
  "use strict";

  const internal = require("internal");
  const db = internal.db;
  const example = require("org/arangodb/graph-examples/example-graph");
  const Person = "UnitTestPerson";
  const Movie = "UnitTestMovie";
  const Acted_In = "UnitTestACTED_IN";
  const Directed = "UnitTestDIRECTED";
  const Wrote = "UnitTestWROTE";
  const Reviewed = "UnitTestREVIEWED";
  const Produced = "UnitTestPRODUCED";

  let graph;

  let cleanup = function () {
    example.dropGraph("movies", true);
  };

  describe("Cypher to AQL comparison", function () {

    beforeEach(function() {
      cleanup();
      graph = example.loadGraph("movies", true);
    });

    afterEach(function() {
      cleanup();
    });

    it("MATCH (tom {name: 'Tom Hanks'}) RETURN tom", function () {
      let query = 'FOR tom IN @@Person FILTER tom.name == @name RETURN tom'; 
      let result = db._query(query, {
        name: "Tom Hanks",
        "@Person": Person
      }).toArray();
      expect(result.length).toEqual(1);
      expect(result[0]._key).toEqual("TomH");
      expect(result[0].name).toEqual("Tom Hanks");
      expect(result[0].born).toEqual(1956);
    });

    it('MATCH (cloudAtlas {title: "Cloud Atlas"}) RETURN cloudAtlas', function () {
      let query = 'FOR cloudAtlas in @@Movie FILTER cloudAtlas.title == @title RETURN cloudAtlas';
      let result = db._query(query, {
        title: "Cloud Atlas",
        "@Movie": Movie
      }).toArray();
      expect(result.length).toEqual(1);
      expect(result[0]._key).toEqual("CloudAtlas");
      expect(result[0].title).toEqual("Cloud Atlas");
      expect(result[0].tagline).toEqual("Everything is connected");
      expect(result[0].released).toEqual(2012);
    });

    it('MATCH (people:Person) RETURN people.name LIMIT 10', function () {
      let query = 'FOR people IN @@Person LIMIT 10 RETURN people.name';
      let result = db._query(query, {
        "@Person": Person
      }).toArray();
      expect(result.length).toEqual(10);
    });

    it('MATCH (nineties:Movie) WHERE nineties.released > 1990 '
      + 'AND nineties.released < 2000 RETURN nineties.title', function() {
        let query = 'FOR nineties IN @@Movie FILTER nineties.released > @early '
        + '&& nineties.released < @late RETURN nineties.title';
      let result = db._query(query, {
        early: 1990,
        late: 2000,
        "@Movie": Movie
      }).toArray();
      expect(result.length).toEqual(19);
      result.sort();
      expect(result[0]).toEqual("A Few Good Men");
      expect(result[1]).toEqual("A League of Their Own");
      expect(result[2]).toEqual("Apollo 13");
      expect(result[3]).toEqual("As Good as It Gets");
      expect(result[4]).toEqual("Bicentennial Man");
      expect(result[5]).toEqual("Hoffa");
      expect(result[6]).toEqual("Johnny Mnemonic");
      expect(result[7]).toEqual("Sleepless in Seattle");
      expect(result[8]).toEqual("Snow Falling on Cedars");
      expect(result[9]).toEqual("That Thing You Do");
      expect(result[10]).toEqual("The Birdcage");
      expect(result[11]).toEqual("The Devil's Advocate");
      expect(result[12]).toEqual("The Green Mile");
      expect(result[13]).toEqual("The Matrix");
      expect(result[14]).toEqual("Twister");
      expect(result[15]).toEqual("Unforgiven");
      expect(result[16]).toEqual("What Dreams May Come");
      expect(result[17]).toEqual("When Harry Met Sally");
      expect(result[18]).toEqual("You've Got Mail");
    });

    it('MATCH (tom:Person {name: "Tom Hanks"})-[:ACTED_IN]->(tomHanksMovies) RETURN tom,tomHanksMovies', function() {
      let query = 'FOR tom IN @@Person FILTER tom.name == @name '
          + 'FOR tomHanksMovies IN TRAVERSE FROM tom GRAPH @@Acted, @@Movie 1 STEPS '
          + 'RETURN {actor: tom, movie: tomHanksMovies.vertex}';
      let result = db._query(query, {
        name: "Tom Hanks",
        "@Person": Person,
        "@Movie": Movie,
        "@Acted": Acted_In
      }).toArray();
      expect(result.length).toEqual(12);
      result.sort(function (a, b) {
        if (a.movie._key < b.movie._key) {
          return -1;
        }
        if (a.movie._key > b.movie._key) {
          return 1;
        }
        return 0;
      });
      expect(result[0].movie._key).toEqual("ALeagueofTheirOwn");
      expect(result[1].movie._key).toEqual("Apollo13");
      expect(result[2].movie._key).toEqual("CastAway");
      expect(result[3].movie._key).toEqual("CharlieWilsonsWar");
      expect(result[4].movie._key).toEqual("CloudAtlas");
      expect(result[5].movie._key).toEqual("JoeVersustheVolcano");
      expect(result[6].movie._key).toEqual("SleeplessInSeattle");
      expect(result[7].movie._key).toEqual("ThatThingYouDo");
      expect(result[8].movie._key).toEqual("TheDaVinciCode");
      expect(result[9].movie._key).toEqual("TheGreenMile");
      expect(result[10].movie._key).toEqual("ThePolarExpress");
      expect(result[11].movie._key).toEqual("YouveGotMail");
    });

    it('MATCH (cloudAtlas {title: "Cloud Atlas"})<-[:DIRECTED]-(directors) RETURN directors.vertex.name', function() {
      let query = 'FOR cloudAtlas IN @@Movie FILTER cloudAtlas.title == @title '
         + 'FOR directors IN TRAVERSE "inbound" FROM cloudAtlas GRAPH @@Directed, @@Person 1 STEPS '
         + 'RETURN directors.vertex.name';
      let result = db._query(query, {
        title: "Cloud Atlas",
        "@Person": Person,
        "@Movie": Movie,
        "@Directed": Directed
      }).toArray();
      expect(result.length).toEqual(3);
      result.sort();
      expect(result[0]).toEqual("Andy Wachowski");
      expect(result[1]).toEqual("Lana Wachowski");
      expect(result[2]).toEqual("Tom Tykwer");
    });

    it('MATCH (tom:Person {name:"Tom Hanks"})-[:ACTED_IN]->(m)<-[:ACTED_IN]-(coActors) '
      + 'RETURN coActors.name', function() {
      let query = 'FOR tom IN @@Person FILTER tom.name == @name '
          + 'FOR m IN TRAVERSE FROM tom GRAPH @@Acted_In, @@Movie 1 STEPS '
          + 'FOR coActors IN TRAVERSE "inbound" FROM m GRAPH @@Acted_In, @@Person 1 STEPS '
          + 'RETURN coActors.vertex';
          // + 'RETURN coActors.vertex.name';
      let result = db._query(query, {
        name: "Tom Hanks",
        "@Person": Person,
        "@Movie": Movie,
        "@Acted_In": Acted_In
      }).toArray();
      require("internal").print(result);
      expect(result.length).toEqual(39);
    });

    it('MATCH (people:Person)-[relatedTo]-(:Movie {title: "Cloud Atlas"})'
      + ' RETURN people.name, Type(relatedTo), relatedTo', function () {
      let query = 'FOR m IN @@Movie FILTER m.title == @title '
      + 'FOR people IN TRAVERSE "any" FROM m GRAPH @graph 1 STEPS '
      + 'FILTER PARSE_IDENTIFIER(people.vertex._id).collection == @Person '
      + 'LET relatedTo = people.path.edges[0] '
      + 'RETURN {'
        + 'name: people.vertex.name,'
        + 'type: PARSE_IDENTIFIER(relatedTo._id).collection,'
        + 'relation: relatedTo'
      + '}';
      let result = db._query(query, {
        title: "Cloud Atlas",
        Person: Person,
        "@Movie": Movie,
        graph: graph.__name
      }).toArray();
      expect(result.length).toEqual(10);
      result.sort(function (a, b) {
        if (a.name < b.name) {
          return -1;
        }
        if (a.name > b.name) {
          return 1;
        }
        return 0;
      });
      expect(result[0].name).toEqual("Andy Wachowski");
      expect(result[0].type).toEqual(Directed);
      expect(result[1].name).toEqual("David Mitchell");
      expect(result[1].type).toEqual(Wrote);
      expect(result[2].name).toEqual("Halle Berry");
      expect(result[2].type).toEqual(Acted_In);
      expect(result[4].name).toEqual("Jessica Thompson");
      expect(result[4].type).toEqual(Reviewed);
      expect(result[7].name).toEqual("Stefan Arndt");
      expect(result[7].type).toEqual(Produced);

    });

    it('MATCH (bacon:Person {name:"Kevin Bacon"})-[*1..4]-(hollywood) RETURN DISTINCT hollywood', function () {
      let query = 'FOR bacon IN @@Person FILTER bacon.name == @name'
        + ' FOR hollywood IN TRAVERSE "any" FROM bacon GRAPH @graph 1..4 STEPS'
        + ' COLLECT h = hollywood.vertex RETURN h';
      let result = db._query(query, {
        name: "Kevin Bacon",
        "@Person": Person,
        graph: graph.__name
      }).toArray();
      expect(result.length).toEqual(135);
    });

    it('MATCH p=shortestPath((bacon:Person {name:"Kevin Bacon"})'
      + '-[*]-(meg:Person {name:"Meg Ryan"})) RETURN p', function () {
      let query = 'FOR bacon IN @@Person FILTER bacon.name == @start '
        + 'FOR meg IN @@Person FILTER meg.name == @end '
        + 'RETURN GRAPH_SHORTEST_PATH(@graph, bacon._id, meg._id, {direction: "any"})';
      let result = db._query(query, {
        start: "Kevin Bacon",
        end: "Meg Ryan",
        "@Person": Person,
        graph: graph.__name
      }).toArray();
      expect(result.length).toEqual(1);
      expect(result[0].distance).toEqual(4);
    });

    it('MATCH (tom:Person {name:"Tom Hanks"})-[:ACTED_IN]->(m)<-[:ACTED_IN]-(coActors),'
      + ' (coActors)-[:ACTED_IN]->(m2)<-[:ACTED_IN]-(cocoActors)'
      + ' WHERE NOT (tom)-[:ACTED_IN]->(m2)'
      + ' RETURN cocoActors.name AS Recommended, count(*) AS Strength ORDER BY Strength DESC', function () {

      let query = 'FOR tom IN @@Person FILTER tom.name == @name '
        + 'LET tomHanksMovies = (FOR m2 IN TRAVERSE FROM tom GRAPH @@Acted_In, @@Movie 1 STEPS RETURN m2.vertex._id) '
        + 'FOR m IN TRAVERSE FROM tom GRAPH @@Acted_In, @@Movie 1 STEPS '
        + 'FOR coActors IN TRAVERSE "inbound" FROM m.vertex GRAPH @@Acted_In, @@Person 1 STEPS '
        + 'FOR m2 IN TRAVERSE FROM coActors.vertex GRAPH @@Acted_In, @@Movie 1 STEPS '
        + 'FILTER m2 NOT IN tomHanksMovies '
        + 'FOR cocoActors IN TRAVERSE "inbound" FROM m2.vertex GRAPH @@Acted_In, @@Person 1 STEPS '
        + 'COLLECT name = cocoActors.vertex.name INTO v '
        + 'SORT LENGTH(v) DESC '
        + 'RETURN { '
        + 'Recommended: name, '
        + 'Strength: LENGTH(v)'
        + '}';
      let result = db._query(query, {
        name: "Tom Hanks",
        "@Acted_In": Acted_In,
        "@Person": Person,
        "@Movie": Movie
      }).toArray();
      // TODO
      expect(result.length).toEqual(50);
      result.sort(function (a, b) {
        if (a.Strength < b.Strength) {
          return -1;
        }
        if (a.Strength > b.Strength) {
          return 1;
        }
        if (a.Recommended < b.Recommended) {
          return -1;
        }
        if (a.Recommended > b.Recommended) {
          return 1;
        }
        return 0;
      });
      expect(result[0].Recommended).toEqual("Tom Cruise");
      expect(result[0].Strength).toEqual(5);
      expect(result[1].Recommended).toEqual("Zach Grenier");
      expect(result[1].Strength).toEqual(5);
      expect(result[2].Recommended).toEqual("Cuba Gooding Jr.");
      expect(result[2].Strength).toEqual(4);
      expect(result[3].Recommended).toEqual("Helen Hunt");
      expect(result[3].Strength).toEqual(4);
      expect(result[4].Recommended).toEqual("Keanu Reeves");
      expect(result[4].Strength).toEqual(4);
    });

  });

}());
