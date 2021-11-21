#include "soc.hpp"

#include "common.hpp"
#include "lib/util/lazy_init.hpp"

namespace exl::util {
    
    namespace {
            
        enum class SocType {
            Erista,
            Mariko
        };

        class SocTypeHolder {
            public:
            SocType m_Type;

            void Initialize() {
                SplHardwareType hwtype;
                R_ABORT_UNLESS(smcGetConfig(SplConfigItem_HardwareType, (u64*)&hwtype));
                
                switch (hwtype) {
                    case SplHardwareType_Icosa:
                    case SplHardwareType_Copper:
                        m_Type = SocType::Erista;
                        return;
                    case SplHardwareType_Hoag:
                    case SplHardwareType_Iowa:
                    case SplHardwareType_Calcio:
                    case SplHardwareType_Aula:
                        m_Type = SocType::Mariko;
                        return;
                        
                    EXL_UNREACHABLE_DEFAULT_CASE();
                }
            }
        };

        LazyInit<SocTypeHolder> m_SocType;
    };

    SocType GetSocType() {
        return m_SocType->m_Type;
    }

    bool IsSocErista() { return GetSocType() == SocType::Erista; }
    bool IsSocMariko() { return GetSocType() == SocType::Mariko; }
}