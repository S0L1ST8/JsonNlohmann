#include <nlohmann/json.hpp>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string_view>

using json = nlohmann::json;

struct CastingRole {
    std::string actor;
    std::string role;
};

struct Movie {
    unsigned int id;
    std::string title;
    unsigned int year;
    unsigned int length;
    std::vector<CastingRole> cast;
    std::vector<std::string> directors;
    std::vector<std::string> writers;
};

using movie_list = std::vector<Movie>;

void to_json(json& j, const CastingRole& c) {
   j = json{
       {"star", c.actor},
       {"name", c.role}
   };
}

void to_json(json& j, const Movie& m) {
   j = json::object({
      {"id", m.id},
      {"title", m.title},
      {"year", m.year},
      {"length", m.length},
      {"cast", m.cast },
      {"directors", m.directors},
      {"writers", m.writers}
   });
}

void Serialize(const movie_list& movies, std::string_view filepath) {
    json jdata{{"movies", movies}};

    std::ofstream ofile(filepath.data());
    if (ofile.is_open()) {
        ofile << std::setw(2) << jdata << std::endl;
    }
}

movie_list Deserialize(std::string_view filepath) {
    movie_list movies;

    std::ifstream ifile(filepath.data());
    if (ifile.is_open()) {
        json jdata;

        try {
            ifile >> jdata;

            if (jdata.is_object()) {
                for (auto & element : jdata.at("movies")) {
                    Movie m;

                    m.id = element.at("id").get<unsigned int>();
                    m.title = element.at("title").get<std::string>();
                    m.year = element.at("year").get<unsigned int>();
                    m.length = element.at("length").get<unsigned int>();

                    for (auto & role : element.at("cast")) {
                        m.cast.push_back(CastingRole{
                                role.at("star").get<std::string>(),
                                role.at("name").get<std::string>()
                        });
                    }

                    for (auto & director : element.at("directors")) {
                        m.directors.push_back(director);
                    }

                    for (auto & writer : element.at("writers")) {
                        m.writers.push_back(writer);
                    }

                    movies.push_back(std::move(m));
                }
            }
        }
        catch (std::exception const & ex) {
            std::cout << ex.what() << std::endl;
        }
    }

    return movies;
}

int main() {
    movie_list movies
    {
       {
          11001,
          "The Matrix",
          1999,
          196,
          {
             {"Keanu Reeves", "Neo"},
             {"Laurence Fishburne", "Morpheus"},
             {"Carrie-Anne Moss", "Trinity"},
             {"Hugo Weaving", "Agent Smith"}
          },
          {"Lana Wachowski", "Lilly Wachowski"},
          {"Lana Wachowski", "Lilly Wachowski"},
       },
       {
          9871,
          "Forrest Gump",
          1994,
          202,
          {
             {"Tom Hanks", "Forrest Gump"},
             {"Sally Field", "Mrs. Gump"},
             {"Robin Wright","Jenny Curran"},
             {"Mykelti Williamson","Bubba Blue"}
          },
          {"Robert Zemeckis"},
          {"Winston Groom", "Eric Roth"},
       }
    };

    Serialize(movies, "../movies.json");

    auto movies_l = Deserialize("../movies.json");

    assert(movies_l.size() == 2);
    assert(movies_l[0].title == "The Matrix");
    assert(movies_l[1].title == "Forrest Gump");
}
