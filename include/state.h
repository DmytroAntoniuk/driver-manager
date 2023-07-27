#pragma once

class State
{
    OSVERSIONINFOEX platform;
    int locale;
    int architecture;

    ofst manuf;
    ofst model;
    ofst product;
    ofst monitors;
    ofst battery;

    ofst windir;
    ofst temp;

    ofst cs_manuf;
    ofst cs_model;
    int ChassisType;
    int revision;
    char reserved[1024];

    char reserved1[676];

    // --- End of POD ---

    loadable_vector<Device> Devices_list;
    loadable_vector<Driver> Drivers_list;

public:
    Txt textas;
    inflist_tp inf_list_new;
    int isLaptop;

private:
    int getbaseboard(WStringShort &manuf, WStringShort &model, WStringShort &product, WStringShort &cs_manuf, WStringShort &cs_model, int *type);
    void fakeOSversion();

public:
    ofst getWindir() const { return windir; }
    ofst getTemp() const { return temp; }
    int getLocale() const { return locale; }
    int getArchitecture() const { return architecture; }
    void getWinVer(int *major, int *minor) const;
    const wchar_t *get_szCSDVersion() const { return platform.szCSDVersion; }
    std::vector<Device> *getDevices_list() { return &Devices_list; }
    const Driver *getCurrentDriver(const Device *dev) const { return (dev->getDriverIndex() >= 0) ? &Drivers_list[dev->getDriverIndex()] : nullptr; }
    WinVersions winVersions;

    const wchar_t *getProduct();
    const wchar_t *getManuf();
    const wchar_t *getModel();
    int getPlatformProductType();

    State();
    void print();
    void popup_sysinfo(Canvas &canvas);
    void contextmenu2(int x, int y);

    int save(const wchar_t *filename);
    int load(const wchar_t *filename);
    void getsysinfo_fast();
    void getsysinfo_slow();
    void getsysinfo_slow(const State *prev);
    void scanDevices();
    void init();

    const wchar_t *get_winverstr();
    size_t opencatfile(const Driver *cur_driver);
    void genmarker();  // in matcher.cpp
    void isnotebook_a();
};
