#pragma once

class Driverpack
{
    ofst drppath;
    ofst drpfilename;

    int type;

    Collection *col;

    Hashtable indexes;
    std::unordered_map<std::string, ofst> cat_list;

    loadable_vector<data_inffile_t> inffile;
    loadable_vector<data_manufacturer_t> manufacturer_list;
    loadable_vector<data_desc_t> desc_list;
    loadable_vector<data_HWID_t> HWID_list;
    Txt text_ind;
    concurrent_queue<inffile_task> *objs_new;

private:
    int genindex();
    void driverpack_parsecat_async(wchar_t const *pathinf, wchar_t const *inffile, const unsigned char *adr, size_t len);
    void driverpack_indexinf_async(wchar_t const *pathinf, wchar_t const *inffile, const unsigned char *adr, size_t len);
    void indexinf_ansi(wchar_t const *drpdir, wchar_t const *inffile, const char *inf_base, size_t inf_len);
    void getdrp_drvsectionAtPos(char *buf, size_t pos, size_t manuf_index);

    static unsigned int __stdcall loaddrp_thread(void *arg);
    static unsigned int __stdcall indexinf_thread(void *arg);
    static unsigned int __stdcall savedrp_thread(void *arg);

public:
    const wchar_t *getPath() const { return text_ind.getw2(drppath); }
    const wchar_t *getFilename() const { return text_ind.getw2(drpfilename); }
    int getType() const { return type; }
    ofst getSize() const { return static_cast<ofst>(HWID_list.size()); }
    int setType(int val) { return type = val; }
    int find(int key, int *isFound) { return indexes.find(key, isFound); }
    int findnext(int *isFound) { return indexes.findnext(isFound); }

    Driverpack(const Driverpack &)            = default;
    Driverpack &operator=(const Driverpack &) = default;
    Driverpack(wchar_t const *driverpack_path, wchar_t const *driverpack_filename, Collection *col);

    int checkindex();
    int loadindex();
    void saveindex();
    void genhashes();
    size_t printstats();
    void print_index_hr();

    void fillinfo(const char *sect, const char *hwid, unsigned start_index, int *inf_pos, ofst *cat, int *catalogfile, int *feature);
    void getindexfilename(const wchar_t *dir, const wchar_t *ext, wchar_t *indfile);
    void parsecat(wchar_t const *pathinf, wchar_t const *inffile, const char *adr, size_t len);
    void indexinf(wchar_t const *drpdir, wchar_t const *inffile, const char *inf_base, size_t inf_len);

    friend class Hwidmatch;
    friend class Collection;
};
