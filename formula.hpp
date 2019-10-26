#include <utility>
#include <vector>
#include <set>
#include <regex>
#include <unordered_map>
#include <iostream>

class Formula;  // Forward declare

enum class ConnectiveType {
    NOT = 0,
    AND,
    OR,
    NAND,
    UNDEFINED,
};

enum class FormulaType {
    ATOM = 0,
    BOOLEAN_OPERATOR
};


using Interpretation = std::unordered_map<Formula*, bool>;


class Formula {
    FormulaType m_formulaType;
    ConnectiveType m_connectiveType;

    std::pair<Formula*, Formula*> m_children;

    std::string m_name;

public:
    Formula(std::string name) : m_name(name) {
        m_connectiveType = ConnectiveType::UNDEFINED;
        m_formulaType = FormulaType::ATOM;
        m_children = std::make_pair(nullptr, nullptr);
    }

    Formula(ConnectiveType type, Formula* left, Formula* right) : m_connectiveType(type) {
        m_formulaType = FormulaType::BOOLEAN_OPERATOR;
        m_children = std::make_pair(left, right);
    }

    ~Formula() {
        if (m_children.first != nullptr) {
            delete m_children.first;
        }
        if (m_children.second != nullptr) {
            delete m_children.second;
        }
    }

    void setLeft(Formula* p) {
        m_children.first = p;
    }

    void setRight(Formula* p) {
        m_children.second = p;
    }

    bool evaluate(Interpretation& i) {
        if (m_formulaType == FormulaType::ATOM) {
            return i[this];

        } else if (m_formulaType == FormulaType::BOOLEAN_OPERATOR) {
            switch (m_connectiveType) {
                case ConnectiveType::NOT :
                    return !m_children.first->evaluate(i);
                
                case ConnectiveType::AND :
                    return m_children.first->evaluate(i) & m_children.second->evaluate(i);

                case ConnectiveType::OR :
                    return m_children.first->evaluate(i) | m_children.second->evaluate(i);

                default :
                    return false;
            }
        }
    }
    
    bool operator == (Formula& other) {
        return (
            this->m_formulaType == other.m_formulaType &&
            this->m_connectiveType == other.m_connectiveType &&
            this->m_children.first == other.m_children.first &&
            this->m_children.second == other.m_children.second
        );
    }

    friend std::ostream& operator<<(std::ostream& os, const Formula& p) {
        if (p.m_formulaType == FormulaType::ATOM) {
            os << p.m_name;

        } else if (p.m_formulaType == FormulaType::BOOLEAN_OPERATOR) {
            switch (p.m_connectiveType) {
                case ConnectiveType::NOT :
                    os << *p.m_children.first << " !";
                    break;

                case ConnectiveType::AND :
                    os << *p.m_children.first << " " << *p.m_children.second << " &";
                    break;
                
                case ConnectiveType::OR :
                    os << *p.m_children.first << " " << *p.m_children.second << " |";
                    break;

                default :
                    os << "NOT-IMPLEMENTED";
                    break;
            }
        }

        return os;
    }
    
};

// Formula factory function
Formula* make_formula(const std::string& rpn_repr, std::unordered_map<std::string, Formula*>& formula_lookup) {
    auto is_operator = [](const std::string& str){
        return std::regex_match (str, std::regex("[!&|^<->]+"));
    };

    std::string tmp;
    std::vector<std::string> tokens;

    // Tokenize the RPN input
    for (const char& current_char : rpn_repr) {
        if (current_char == ' ') {
            tokens.push_back(tmp);
            tmp = "";
        } else {
            tmp += current_char;
        }
    }

    if (tmp != "") {
        tokens.push_back(tmp);
    }

    Formula* temp = nullptr;
    std::vector<Formula*> subformulas;

    for (const auto& token : tokens) {
        if (!is_operator(token)) {
            if (formula_lookup.find(token) == formula_lookup.end()) {
                temp = new Formula(token);
                formula_lookup[token] = temp;
            }

        } else {
            if (token == "&") {
                Formula* f1 = subformulas.back();
                subformulas.pop_back();
                Formula* f2 = subformulas.back();
                subformulas.pop_back();
                temp = new Formula(ConnectiveType::AND, f1, f2);

            } else if (token == "!") {
                Formula* f1 = subformulas.back();
                subformulas.pop_back();

                temp = new Formula(ConnectiveType::NOT, f1, nullptr); 
            }
        }

        subformulas.push_back(temp);
    }

    return subformulas.back();
}
