#include <dlfcn.h>
#include <iostream>
#include <elf.h>
#include <link.h>
#include <cxxabi.h>
#include <string.h>
#include <unordered_map>
#include <memory>
#include <string>

#include "shared.h"


class CStatechartExecutor
{
public:
    using func_raiseEvent_t = void (*)(CMyClass *);
    using func_cmyclass_ctor_t = CMyClass* (*)();

    int init(std::string &library_name)
    {
        void *library_handle;
        library_handle = dlopen(library_name.c_str(), RTLD_LAZY);

        if (!library_handle) {
            std::cerr << "Error: " << dlerror() << std::endl;
            return -1;
        }

        struct link_map *map = nullptr;
        dlinfo(library_handle, RTLD_DI_LINKMAP, &map);

        Elf64_Sym *symtab = nullptr;
        char *strtab = nullptr;
        int symentries = 0;

        for (auto section = map->l_ld; section->d_tag != DT_NULL; ++section) {
            if (section->d_tag == DT_SYMTAB) {
                symtab = (Elf64_Sym *)section->d_un.d_ptr;
            }
            if (section->d_tag == DT_STRTAB) {
                strtab = (char*)section->d_un.d_ptr;
            }
            if (section->d_tag == DT_SYMENT) {
                symentries = section->d_un.d_val;
            }
        }

        int size = strtab - (char *)symtab;
        for (int k = 0; k < size / symentries; ++k) {
            auto sym = &symtab[k];
            if (ELF64_ST_TYPE(symtab[k].st_info) == STT_FUNC) {
                char *str = &strtab[sym->st_name];

                int status = -1;
                char *demangled_name = abi::__cxa_demangle(str, NULL, NULL, &status);
                if (demangled_name) {
                    std::string strDemangledName(demangled_name);
                    std::cout << strDemangledName << std::endl;
                    //if (strDemangledName == "CMyClass::CMyClass()") <- for some reason direct call to ctor doesn't work
                    if (strDemangledName == "get_instance()") {
                        func_cmyclass_ctor_t ctor = (func_cmyclass_ctor_t)dlsym(library_handle, str);
                        m_pStatechartObject = std::make_unique<CMyClass>(*ctor());
                        continue;
                    }
                    if (strDemangledName.rfind("CMyClass::raiseEvent") == 0) {
                        func_raiseEvent_t func_raiseEvent = (func_raiseEvent_t)dlsym(library_handle, str);
                        if (!func_raiseEvent) {
                            std::free(demangled_name);
                            continue;
                        }
                        m_mFunctionsTable[strDemangledName] = std::make_unique<func_raiseEvent_t>(func_raiseEvent);
                    }
                    std::free(demangled_name);
                }
            }
        }
        return 0;
    }

    bool exec_procedure(const std::string &procedure_name)
    {
        auto func = m_mFunctionsTable.find(procedure_name);
        if (func != m_mFunctionsTable.end()) {
            (*func->second)(m_pStatechartObject.get());
            return true;
        }
        return false;
    }

private:
    std::unique_ptr<CMyClass> m_pStatechartObject;
    std::unordered_map<std::string, std::unique_ptr<func_raiseEvent_t>> m_mFunctionsTable;

};


int main()
{
    CStatechartExecutor e;
    std::string library_name("./libshared.so");
    e.init(library_name);
    e.exec_procedure("CMyClass::raiseEvent1()");
    e.exec_procedure("CMyClass::raiseEvent2()");
    e.exec_procedure("CMyClass::raiseEvent3()");
    e.exec_procedure("CMyClass::raiseEvent1()");

    return EXIT_SUCCESS;
}
