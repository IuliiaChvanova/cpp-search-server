#include "search_server.h"
    
void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status,
                    const std::vector<int>& ratings) {
    if (document_id < 0){
        throw std::invalid_argument("Попытка добавить документ с отрицательным id");
    }
    if (documents_.count(document_id) > 0){
        throw std::invalid_argument("Попытка добавить документ c id ранее добавленного документа");
    }
    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    for (const auto& word : words){
        if (!IsValidWord(word)){
            throw std::invalid_argument("Наличие недопустимых символов (с кодами от 0 до 31) в тексте добавляемого документа");
        }
    } 
    const double inv_word_count = 1.0 / words.size();
    for (const std::string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    documents_id_.push_back(document_id);
}



std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(
        raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
        });
        
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
    const Query query = ParseQuery(raw_query);
    //isQueryCorrect(query);
    std::vector<std::string> matched_words;
    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return {matched_words, documents_.at(document_id).status};
}

int SearchServer::GetDocumentId(int index) const {   
    return documents_id_.at(index);
}  



bool SearchServer::IsValidWord(const std::string& word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = accumulate(ratings.begin(), ratings.end(), 0); // changed
    return rating_sum / static_cast<int>(ratings.size());
}


SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    if (!IsValidWord(text)){
        throw std::invalid_argument("В словах поискового запроса есть недопустимые символы с кодами от 0 до 31");
    }
    if(text[text.size() - 1] == '-'){
                throw std::invalid_argument("Наличие знака '-' в конце слова");
        }    
    bool is_minus = false;
    // Word shouldn't be empty
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    if (text.empty()){
        throw std::invalid_argument("Отсутствие текста после символа «минус» в поисковом запросе");
    }
    if (text[1] == '-' || text[0] == '-'){
        throw std::invalid_argument("Наличие более чем одного минуса перед минус словами");
    }
    return {text, is_minus, IsStopWord(text)};
}




SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    Query query;
    for (const std::string& word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            } else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}

// Existence required
double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}