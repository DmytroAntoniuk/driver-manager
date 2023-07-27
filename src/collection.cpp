#include "pch.h"

#include "collection.h"

int Collection::scanfolder_count(const wchar_t *path)
{
    int cnt = 0;

    WStringShort buf;
    buf.sprintf(L"%s\\*.*", path);
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(buf.Get(), &FindFileData);
    while (FindNextFile(hFind, &FindFileData) != 0)
    {
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (lstrcmp(FindFileData.cFileName, L"..") == 0)
                continue;
            buf.sprintf(L"%s\\%ws", path, FindFileData.cFileName);
            if (!loaded_unpacked)
                cnt += scanfolder_count(buf.Get());
        }
        else
        {
            size_t i, len = wcslen(FindFileData.cFileName);
            if (len < 3)
                continue;

            for (i = 0; i < 6; i++)
                if (StrStrIW(FindFileData.cFileName, olddrps[i]))
                {
                    if (Settings.flags & (FLAG_AUTOINSTALL | FLAG_NOGUI))
                        break;
                    buf.sprintf(L" /c del \"%s\\%s*.7z\" /Q /F", driverpack_dir, olddrps[i]);
                    System.run_command(L"cmd", buf.Get(), SW_HIDE, 1);
                    break;
                }
            if (i == 6 && _wcsicmp(FindFileData.cFileName + len - 3, L".7z") == 0)
            {
                Driverpack drp{path, FindFileData.cFileName, this};
                if (Settings.flags & COLLECTION_FORCE_REINDEXING || !drp.checkindex())
                    cnt++;
            }
        }
    }
    FindClose(hFind);
    return cnt;
}

void Collection::scanfolder(const wchar_t *path, void *arg)
{
    WStringShort buf;
    buf.sprintf(L"%s\\*.*", path);
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind   = FindFirstFile(buf.Get(), &FindFileData);
    size_t pathlen = wcslen(driverpack_dir) + 1;

    while (FindNextFile(hFind, &FindFileData) != 0)
    {
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (lstrcmp(FindFileData.cFileName, L"..") == 0)
                continue;
            buf.sprintf(L"%s\\%ws", path, FindFileData.cFileName);
            if (!loaded_unpacked)
                scanfolder(buf.Get(), arg);
        }
        else
        {
            size_t len = wcslen(FindFileData.cFileName);
            if (len < 3)
                continue;

            if (_wcsicmp(FindFileData.cFileName + len - 3, L".7z") == 0)
            {
                driverpack_list.push_back(Driverpack(path, FindFileData.cFileName, this));
                reinterpret_cast<drplist_t *>(arg)->push(driverpack_task{&driverpack_list.back()});
            }
            else if ((_wcsicmp(FindFileData.cFileName + len - 4, L".inf") == 0 || _wcsicmp(FindFileData.cFileName + len - 4, L".cat") == 0) && loaded_unpacked == 0)
            {
                buf.sprintf(L"%s\\%s", path, FindFileData.cFileName);
                FILE *f = _wfopen(buf.Get(), L"rb");
                _fseeki64(f, 0, SEEK_END);
                len = static_cast<size_t>(_ftelli64(f));
                _fseeki64(f, 0, SEEK_SET);
                char *buft = new char[len];
                fread(buft, len, 1, f);
                fclose(f);
                buf.sprintf(L"%s\\", path + pathlen);

                if (len)
                {
                    if (_wcsicmp(FindFileData.cFileName + wcslen(FindFileData.cFileName) - 4, L".inf") == 0)
                        driverpack_list[0].indexinf(buf.Get(), FindFileData.cFileName, buft, len);
                    else
                        driverpack_list[0].parsecat(buf.Get(), FindFileData.cFileName, buft, len);
                }

                delete[] buft;
            }
        }
    }
    FindClose(hFind);
}

void Collection::loadOnlineIndexes()
{
    // load online indexes for driver packs
    // that i don't have
    WStringShort buf;
    ;
    buf.sprintf(L"%s\\_*.*", index_bin_dir);
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(buf.Get(), &FindFileData);

    while (FindNextFile(hFind, &FindFileData) != 0)
    {
        wchar_t filename[BUFLEN];
        wsprintf(filename, L"%ws", FindFileData.cFileName);
        wcscpy(filename + wcslen(FindFileData.cFileName) - 3, L"7z");

        buf.sprintf(L"drivers\\%ws", filename);
        (buf.GetV())[8] = L'D';
        if (System.FileExists(buf.Get()))
        {
            Log.print_debug("Skip %S\n", buf.Get());
            continue;
        }

        driverpack_list.push_back(Driverpack(driverpack_dir, filename, this));
        driverpack_list.back().loadindex();
    }
    FindClose(hFind);
}

void Collection::init(const wchar_t *driverpacks_dirv, const wchar_t *index_bin_dirv, const wchar_t *index_linear_dirv)
{
    driverpack_dir   = driverpacks_dirv;
    index_bin_dir    = index_bin_dirv;
    index_linear_dir = index_linear_dirv;
    driverpack_list.clear();
}

Collection::Collection(const wchar_t *driverpacks_dirv, const wchar_t *index_bin_dirv, const wchar_t *index_linear_dirv)
{
    driverpack_dir   = driverpacks_dirv;
    index_bin_dir    = index_bin_dirv;
    index_linear_dir = index_linear_dirv;
}

void Collection::updatedir()
{
    driverpack_dir = *Settings.drpext_dir ? Settings.drpext_dir : Settings.drp_dir;
    populate();
}

void Collection::populate()
{
    Log.print_debug("Collection::populate\n");
    Driverpack *unpacked_drp;

    Timers.start(time_indexes);

    driverpack_list.reserve(drp_count + 1 + 300);  // TODO
    driverpack_list.push_back(Driverpack(driverpack_dir, L"unpacked.7z", this));
    unpacked_drp = &driverpack_list.back();

    if (Settings.flags & FLAG_KEEPUNPACKINDEX)
        loaded_unpacked = unpacked_drp->loadindex();
    drp_count = scanfolder_count(driverpack_dir);

    if ((Settings.flags & FLAG_KEEPUNPACKINDEX) && !loaded_unpacked)
        manager_g->itembar_settext(SLOT_INDEXING, 1, L"Unpacked", 1, ++drp_count, 0);

    //{thread
    drplist_t queuedriverpack1;
    queuedriverpack_p = &queuedriverpack1;
    int num_thr       = num_cores;
    int num_thr_1     = num_cores;
#ifndef _WIN64
    if (drp_count && num_thr > 1)
        num_thr = 1;
#endif

    Log.print_debug("Collection::populate::num_thr::%d\n", num_thr);

    drplist_t queuedriverpack;
    ThreadAbs *cons[num_thr];
    for (int i = 0; i < num_thr; i++)
    {
        Log.print_debug("Collection::populate::ThreadAbs::%d\n", i);
        cons[i] = CreateThread();
        cons[i]->start(&Driverpack::loaddrp_thread, &queuedriverpack);
    }

    Log.print_debug("Collection::populate::num_thr_1::%d\n", num_thr_1);
    ThreadAbs *thr[num_thr_1];
    for (int i = 0; i < num_thr_1; i++)
    {
        Log.print_debug("Collection::populate::ThreadAbs1::%d\n", i);
        thr[i] = CreateThread();
        thr[i]->start(&Driverpack::indexinf_thread, &queuedriverpack1);
    }
    //}thread

    drp_cur = 1;

    Log.print_debug("Collection::populate::scanfolder::%S\n", driverpack_dir);
    scanfolder(driverpack_dir, &queuedriverpack);
    for (int i = 0; i < num_thr; i++)
    {
        Log.print_debug("Collection::populate::queuedriverpack.push::%d\n", i);
        queuedriverpack.push(driverpack_task{nullptr});
    }

    for (int i = 0; i < num_thr; i++)
    {
        Log.print_debug("Collection::populate::cons[i]->join::%d\n", i);
        cons[i]->join();
        delete cons[i];
    }

    Log.print_debug("Collection::populate::loadOnlineIndexes\n");
    loadOnlineIndexes();

    Log.print_debug("Collection::populate::itembar\n");
    manager_g->itembar_setactive(SLOT_INDEXING, 0);
    if (driverpack_list.size() <= 1 && (Settings.flags & FLAG_DPINSTMODE) == 0)
    {
        emptydrp = true;
        if ((Settings.flags & FLAG_CHECKUPDATES) == 0)
            manager_g->itembar_settext(SLOT_NODRIVERS, L"", 0);
    }
    else
        emptydrp = false;

    Log.print_debug("Collection::populate::genhashes\n");
    driverpack_list[0].genhashes();

    //{thread
    Log.print_debug("Collection::populate::queuedriverpack1\n");
    for (int i = 0; i < num_thr_1; i++)
        queuedriverpack1.push(driverpack_task{nullptr});

    for (int i = 0; i < num_thr_1; i++)
    {
        thr[i]->join();
        delete thr[i];
    }
    //}thread
    Settings.flags &= ~COLLECTION_FORCE_REINDEXING;
    Log.print_debug("Collection::populate::driverpack_list.shrink_to_fit\n");
    driverpack_list.shrink_to_fit();
    Timers.stop(time_indexes);
    Log.print_debug("Collection::populate::Done\n");
}

void Collection::save()
{
    if (*Settings.drpext_dir)
        return;
    if (!System.canWriteDirectory(index_bin_dir))
    {
        Log.print_err("ERROR in collection_save(): Write-protected,'%S'\n", index_bin_dir);
        return;
    }
    Timers.start(time_indexsave);

    // Save indexes
    count_ = 0;
    cur_   = 1;
    if ((Settings.flags & FLAG_KEEPUNPACKINDEX) == 0)
        driverpack_list[0].setType(DRIVERPACK_TYPE_INDEXED);
    for (auto &driverpack : driverpack_list)
        if (driverpack.getType() == DRIVERPACK_TYPE_PENDING_SAVE)
            count_++;

    if (count_)
        Log.print_con("Saving indexes...\n");
    ThreadAbs *thr[num_cores];
    drplist_t queuedriverpack_loc;
    for (int i = 0; i < num_cores; i++)
    {
        thr[i] = CreateThread();
        thr[i]->start(&Driverpack::savedrp_thread, &queuedriverpack_loc);
    }
    for (auto &driverpack : driverpack_list)
        if (driverpack.getType() == DRIVERPACK_TYPE_PENDING_SAVE)
            queuedriverpack_loc.push(driverpack_task{&driverpack});

    for (int i = 0; i < num_cores; i++)
        queuedriverpack_loc.push(driverpack_task{nullptr});
    for (int i = 0; i < num_cores; i++)
    {
        thr[i]->join();
        delete thr[i];
    }
    manager_g->itembar_settext(SLOT_INDEXING, 0);
    if (count_)
        Log.print_con("DONE\n");

    // Delete unused indexes
    WIN32_FIND_DATA FindFileData;
    WStringShort filename;
    WStringShort buf;
    buf.sprintf(L"%ws\\*.*", index_bin_dir);
    HANDLE hFind = FindFirstFile(buf.Get(), &FindFileData);
    while (FindNextFile(hFind, &FindFileData) != 0)
    {
        if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if (FindFileData.cFileName[0] == L'_')
                continue;
            filename.sprintf(L"%s\\%s", index_bin_dir, FindFileData.cFileName);
            size_t i;
            for (i = Settings.flags & FLAG_KEEPUNPACKINDEX ? 0 : 1; i < driverpack_list.size(); i++)
            {
                wchar_t buf1[BUFLEN];
                driverpack_list[i].getindexfilename(index_bin_dir, L"bin", buf1);
                if (!_wcsicmp(buf1, filename.Get()))
                    break;
            }
            if (i == driverpack_list.size())
            {
                Log.print_con("Deleting %S\n", filename.Get());
                _wremove(filename.Get());
            }
        }
    }
    Timers.stop(time_indexsave);
}

void Collection::printstats()
{
    if (Log.isHidden(LOG_VERBOSE_DRP))
        return;

    size_t sum = 0;
    Log.print_file("Driverpacks\n");
    for (auto &drp : driverpack_list)
        sum += drp.printstats();

    Log.print_file("  Sum: %d\n\n", sum);
}

void Collection::print_index_hr()
{
    Timers.start(time_indexprint);

    for (auto &drp : driverpack_list)
        drp.print_index_hr();

    Timers.stop(time_indexprint);
}

const wchar_t *Collection::finddrp(const wchar_t *fnd)
{
    int j;
    const wchar_t *s, *d, *n_s;

    j   = 0;
    n_s = nullptr;
    for (auto &drp : driverpack_list)
    {
        s = drp.getFilename();
        if (StrStrIW(s, fnd) && drp.getType() != DRIVERPACK_TYPE_UPDATE)
        {
            d = s;
            while (*d)
            {
                if (*d == L'_' && d[1] >= L'0' && d[1] <= L'9')
                {
                    if (j < _wtoi_my(d + 1))
                    {
                        j   = _wtoi_my(d + 1);
                        n_s = s;
                    }
                    break;
                }
                d++;
            }
        }
    }
    return n_s;
}
//}
