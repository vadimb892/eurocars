#include "general.h"

HRESULT tryInitEx() {
    HRESULT hres;
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        std::cout << "Failed to initialize COM library. Error code = 0x"
            << std::hex << hres << std::endl;
        return -1;              
    }
    return hres;
}

HRESULT tryInitSecurity() {
    HRESULT hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );


    if (FAILED(hres))
    {
        std::cout << "Failed to initialize security. Error code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return -1;                   
    }
    return hres;
}

HRESULT tryCreateInstance(IWbemLocator*& pLoc) {
    HRESULT hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        std::cout << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return -1;            
    }
    return hres;
}

HRESULT tryConnectServer(IWbemLocator*& pLoc, IWbemServices*& pSvc) {
    HRESULT hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
        NULL,                    // User name. NULL = current user
        NULL,                    // User password. NULL = current
        0,                       // Locale. NULL indicates current
        NULL,                    // Security flags.
        0,                       // Authority (for example, Kerberos)
        0,                       // Context object 
        &pSvc                    // pointer to IWbemServices proxy
    );

    if (FAILED(hres))
    {
        std::cout << "Could not connect. Error code = 0x"
            << std::hex << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return -1;               
    }

    std::cout << "Connected to ROOT\\CIMV2 WMI namespace" << std::endl;
    return hres;
}

HRESULT trySetProxyBlanket(IWbemLocator*& pLoc, IWbemServices*& pSvc) {
    HRESULT hres = CoSetProxyBlanket(
        pSvc,                        
        RPC_C_AUTHN_WINNT,          
        RPC_C_AUTHZ_NONE,           
        NULL,                        
        RPC_C_AUTHN_LEVEL_CALL,      
        RPC_C_IMP_LEVEL_IMPERSONATE, 
        NULL,                       
        EOAC_NONE                    
    );

    if (FAILED(hres))
    {
        std::cout << "Could not set proxy blanket. Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return -1;            
    }
    return hres;
}

HRESULT makeQuery(IWbemLocator*& pLoc,
                  IWbemServices*& pSvc,
                  IEnumWbemClassObject*& pEnumerator){
    HRESULT hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_LogicalDisk"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        std::cout << "Query for operating system name failed."
            << " Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return -1;              
    }
    return hres;
}

std::map<std::wstring, DiskAttrTypes> localDiskAttr = {
        {L"Access", DINT},
        {L"Availability", DINT},
        {L"BlockSize", DLONG},
        {L"Caption", DSTR},
        {L"Compressed", DBOOL},
        {L"ConfigManagerErrorCode", DINT},
        {L"ConfigManagerUserConfig", DBOOL},
        {L"CreationClassName", DSTR},
        {L"Description", DSTR},
        {L"DeviceID", DSTR},
        {L"DriveType", DINT},
        {L"ErrorCleared", DBOOL},
        {L"ErrorDescription", DSTR},
        {L"ErrorMethodology", DSTR},
        {L"FileSystem", DSTR},
        {L"FreeSpace", DLONG},
        {L"InstallDate", DDATE},
        {L"LastErrorCode", DINT},
        {L"MaximumComponentLength",DINT},
        {L"MediaType", DINT},
        {L"Name", DSTR},
        {L"NumberOfBlocks", DLONG},
        {L"PNPDeviceID", DSTR},
        {L"PowerManagementCapabilities", DINT},
        {L"PowerManagementSupported", DBOOL},
        {L"ProviderName", DSTR},
        {L"Purpose", DSTR},
        {L"QuotasDisabled", DBOOL},
        {L"QuotasIncomplete", DBOOL},
        {L"QuotasRebuilding", DBOOL},
        {L"Size", DSTR},
        {L"Status", DSTR},
        {L"StatusInfo", DINT},
        {L"SupportsDiskQuotas", DBOOL},
        {L"SupportsFileBasedCompression", DBOOL},
        {L"SystemCreationClassName", DSTR},
        {L"SystemName", DSTR},
        {L"VolumeDirty", DBOOL},
        {L"VolumeName", DSTR},
        {L"VolumeSerialNumber", DSTR}
};

void printDiskAttr(std::vector<std::wstring> fieldsVector, IWbemClassObject*& pclsObj) {
    VARIANT vtProp;
    TCHAR* tc = new TCHAR[MAX_PATH];
    for (std::wstring s : fieldsVector) {
        _tcscpy_s(tc, 50, s.c_str());
        pclsObj->Get(tc, 0, &vtProp, 0, 0);
        switch (localDiskAttr[s]) {
        case D: {
            return;
        }
        case DINT: {
            if (vtProp.uintVal == NULL) {
                std::wcout << std::wstring(CELL_SIZE - 1, ' ') << "-";
                break;
            }
            std::wcout << std::wstring(CELL_SIZE - sizeof(vtProp.uintVal), ' ');
            std::wcout << vtProp.uintVal;
            break;
        };
        case DLONG: {
            if (vtProp.ulVal == NULL) {
                std::wcout << std::wstring(CELL_SIZE - 1, ' ') << "-";
                break;
            }
            std::wcout << std::wstring(CELL_SIZE - sizeof(vtProp.ulVal), ' ');
            std::wcout << vtProp.ulVal;
            break;
        };
        case DSTR: {
            if (vtProp.bstrVal == NULL) {
                std::wcout << std::wstring(CELL_SIZE - 1, ' ') << "-";
                break;
            }
            std::wcout << std::wstring(CELL_SIZE - sizeof(vtProp.bstrVal), ' ');
            std::wcout << vtProp.bstrVal;
            break;
        };
        case DBOOL: {
            if (vtProp.boolVal == NULL) {
                std::wcout << std::wstring(CELL_SIZE - 1, ' ') << "-";
                break;
            }
            std::wcout << std::wstring(CELL_SIZE - sizeof(vtProp.boolVal), ' ');
            std::wcout << vtProp.boolVal;
            break;
        };
        case DDATE: {
            if (vtProp.date == NULL) {
                std::wcout << std::wstring(CELL_SIZE - 1, ' ') << "-";
                break;
            }
            std::wcout << std::wstring(CELL_SIZE - sizeof(vtProp.date), ' ');
            std::wcout << vtProp.date;
            break;
        };
        }
    }
    VariantClear(&vtProp);
}

void getLogicDiskData(IEnumWbemClassObject* pEnumerator){
    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;
    std::vector<std::wstring> fieldVector;
    std::wstring padding;
    std::wstring field;

    fieldVector.push_back(L"Name");
    std::wcout << "Enter fields:\n(\n    ";
    while (true) {
        std::wcin >> field;
        if (field[field.size() - 1] != ',') {
            break;
        }
        field.erase(field.size()-1);
        if (localDiskAttr.find(field) == localDiskAttr.end()) {
            break;
        }
        fieldVector.push_back(field.c_str());
        std::cout << "    ";
    }
    if (fieldVector.size() == 1 && localDiskAttr.find(field) != localDiskAttr.end())
        fieldVector.push_back(field.c_str());
    std::cout << ")\n\n";

    for (std::wstring field : fieldVector) {
        padding = std::wstring((CELL_SIZE - field.size()) / 2, ' ');
        std::wcout << padding << field << padding;
    }
    std::wcout << "\n";

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
            &pclsObj, &uReturn);

        if (0 == uReturn)
            break;

        printDiskAttr(fieldVector, pclsObj);
        std::wcout << std::endl;
        pclsObj->Release();
    }
}

void uninitialize(IWbemLocator* pLoc,
                  IWbemServices* pSvc,
                  IEnumWbemClassObject* pEnumerator) {
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();
}

void task4()
{
    IWbemLocator* pLoc = NULL;
    IWbemServices* pSvc = NULL;
    IEnumWbemClassObject* pEnumerator = NULL;

    assert(FAILURE != tryInitEx());
    assert(FAILURE != tryInitSecurity());
    assert(FAILURE != tryCreateInstance(pLoc));
    assert(FAILURE != tryConnectServer(pLoc, pSvc));
    assert(FAILURE != trySetProxyBlanket(pLoc, pSvc));
    assert(FAILURE != makeQuery(pLoc, pSvc, pEnumerator));

    getLogicDiskData(pEnumerator);
    uninitialize(pLoc, pSvc, pEnumerator);  
}