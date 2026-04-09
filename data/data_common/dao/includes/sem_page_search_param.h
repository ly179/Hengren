#ifndef SEM_PAGE_SEARCH_PARAM_H
#define SEM_PAGE_SEARCH_PARAM_H
class SemPageSearchParam {
private:
    int page = 1;
    int pageSize = 10;
public:
    SemPageSearchParam() = default;
    SemPageSearchParam(const SemPageSearchParam& param) {
        *this = param;
    }
    int getPage() const {
        return this->page;
    }
    void setPage(int page) {
        this->page = page;
    }
    int getPageSize() const {
        return pageSize;
    }
    void setPageSize(int pageSize) {
        this->pageSize = pageSize;
    }
    SemPageSearchParam& operator=(const SemPageSearchParam& other) {
        this->page = other.page;
        this->pageSize = other.pageSize;
        return *this;
    }
};

#endif // SEM_PAGE_SEARCH_PARAM_H
