#pragma once

class Collection
{
    const wchar_t *index_bin_dir;
    const wchar_t *index_linear_dir;
    std::vector<Driverpack> driverpack_list;
    const wchar_t *driverpack_dir;

private:
    int scanfolder_count(const wchar_t *path);
    void scanfolder(const wchar_t *path, void *arg);
    void loadOnlineIndexes();

public:
    const wchar_t *getDriverpack_dir() const { return driverpack_dir; }
    const wchar_t *getIndex_bin_dir() const { return index_bin_dir; }
    const wchar_t *getIndex_linear_dir() const { return index_linear_dir; }
    size_t size() { return driverpack_list.size(); }
    std::vector<Driverpack> *getList() { return &driverpack_list; }

    void init(const wchar_t *driverpacks_dir, const wchar_t *index_bin_dir, const wchar_t *index_linear_dir);
    Collection(const wchar_t *driverpacks_dir, const wchar_t *index_bin_dir, const wchar_t *index_linear_dir);
    Collection() : index_bin_dir(nullptr), index_linear_dir(nullptr), driverpack_dir(nullptr) {}

    void updatedir();
    void populate();
    void save();
    void printstats();
    void print_index_hr();

    const wchar_t *finddrp(const wchar_t *s);
};
