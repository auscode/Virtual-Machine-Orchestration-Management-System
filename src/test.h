#include <cxxtest/TestSuite.h>
#include "../src/bitmap.h"

class SampleTestSuite : public CxxTest::TestSuite
{
public:
    void testInitVmoSystem_ValidNumVms()
    {
        VMO_System system = init_vmo_system(3);
        TS_ASSERT(system.vms != NULL);
        TS_ASSERT_EQUALS(system.num_vms, 3);
        TS_ASSERT_EQUALS(system.vms[0].id, 0);
        TS_ASSERT_EQUALS(system.vms[1].id, 1);
        TS_ASSERT_EQUALS(system.vms[2].id, 2);
        TS_ASSERT_EQUALS(strcmp(system.vms[0].name, "VM0"), 0);
        TS_ASSERT_EQUALS(strcmp(system.vms[1].name, "VM1"), 0);
        TS_ASSERT_EQUALS(strcmp(system.vms[2].name, "VM2"), 0);
        TS_ASSERT_EQUALS(system.vms[0].state, VM_STATE_STOPPED);
        TS_ASSERT_EQUALS(system.vms[1].state, VM_STATE_STOPPED);
        TS_ASSERT_EQUALS(system.vms[2].state, VM_STATE_STOPPED);
    }
    void testInitVMOSystem_ZeroNumVMs()
    {
        VMO_System system = init_vmo_system(0);
        TS_ASSERT(system.vms != NULL);
        TS_ASSERT_EQUALS(system.num_vms, 0);
    }
    void testInitVMOSystem_PositiveNumVMs()
    {
        VMO_System system = init_vmo_system(5);
        TS_ASSERT(system.vms != NULL);
        TS_ASSERT_EQUALS(system.num_vms, 5);
        for (int i = 0; i < system.num_vms; i++)
        {
            TS_ASSERT_EQUALS(system.vms[i].id, i);
            TS_ASSERT_EQUALS(strcmp(system.vms[i].name, "VM") == 0, false);
            TS_ASSERT_EQUALS(system.vms[i].state, VM_STATE_STOPPED);
        }
        free(system.vms);
    }
    ////////////////////////////////////////////////////////////////
    void test_add_vm_with_null_name()
    {
        VMO_System vmo = init_vmo_system(0);
        int result = add_vm(&vmo, NULL);
        TS_ASSERT_EQUALS(result, -1);
        TS_ASSERT_EQUALS(vmo.num_vms, 0);
    }

    void testAddVm_Success()
    {
        VMO_System vmo = init_vmo_system(0);
        char name[] = "vm1";
        int result = add_vm(&vmo, name);
        TS_ASSERT_EQUALS(result, 1);
        TS_ASSERT_EQUALS(vmo.num_vms, 1);
        TS_ASSERT_EQUALS(vmo.vms[0].id, 1);
        TS_ASSERT_EQUALS(std::string(vmo.vms[0].name), "vm1");
        TS_ASSERT_EQUALS(vmo.vms[0].state, VM_STATE_STOPPED);
    }

    void testAddVm_MultipleSuccess()
    {
        VMO_System vmo = init_vmo_system(0);
        char name1[] = "vm1";
        char name2[] = "vm2";
        char name3[] = "vm3";
        int result1 = add_vm(&vmo, name1);
        int result2 = add_vm(&vmo, name2);
        int result3 = add_vm(&vmo, name3);
        TS_ASSERT_EQUALS(result1, 1);
        TS_ASSERT_EQUALS(result2, 2);
        TS_ASSERT_EQUALS(result3, 3);
        TS_ASSERT_EQUALS(vmo.num_vms, 3);
        TS_ASSERT_EQUALS(vmo.vms[0].id, 1);
        TS_ASSERT_EQUALS(std::string(vmo.vms[0].name), "vm1");
        TS_ASSERT_EQUALS(vmo.vms[0].state, VM_STATE_STOPPED);
        TS_ASSERT_EQUALS(vmo.vms[1].id, 2);
        TS_ASSERT_EQUALS(std::string(vmo.vms[1].name), "vm2");
        TS_ASSERT_EQUALS(vmo.vms[1].state, VM_STATE_STOPPED);
        TS_ASSERT_EQUALS(vmo.vms[2].id, 3);
        TS_ASSERT_EQUALS(std::string(vmo.vms[2].name), "vm3");
        TS_ASSERT_EQUALS(vmo.vms[2].state, VM_STATE_STOPPED);
    }

    ///////////////////////////////////////////////////////////

    void testRemoveVM_InvalidVMOSystem()
    {
        VMO_System vmo = {NULL, 0};
        int result = remove_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -1);
    }
    void testRemoveVM_FirstVM()
    {
        VMO_System vmo = {(VM *)malloc(2 * sizeof(VM)), 2};
        vmo.vms[0] = (VM){1, "VM1", VM_STATE_RUNNING};
        vmo.vms[1] = (VM){2, "VM2", VM_STATE_STOPPED};
        int result = remove_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, 0);
        TS_ASSERT_EQUALS(vmo.num_vms, 1);
        TS_ASSERT_EQUALS(vmo.vms[0].id, 2);
        free(vmo.vms);
    }
    void testRemoveVM_LastVM()
    {
        VMO_System vmo = {(VM *)malloc(2 * sizeof(VM)), 2};
        vmo.vms[0] = (VM){1, "VM1", VM_STATE_RUNNING};
        vmo.vms[1] = (VM){2, "VM2", VM_STATE_STOPPED};
        int result = remove_vm(&vmo, 2);
        TS_ASSERT_EQUALS(result, 0);
        TS_ASSERT_EQUALS(vmo.num_vms, 1);
        TS_ASSERT_EQUALS(vmo.vms[0].id, 1);
        free(vmo.vms);
    }
    //////////////////////////////////////////////////////////////////
    
    void testStartVM_NullVMOSystem()
    {
        VMO_System vmo = {NULL, 0};
        int result = start_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -1);
    }

    void testStartVM_AlreadyRunning()
    {
        VMO_System vmo = {(VM *)malloc(2 * sizeof(VM)), 2};
        vmo.vms[0].id = 1;
        vmo.vms[0].state = VM_STATE_RUNNING;
        int result = start_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -3);
        free(vmo.vms);
    }

    void testStartVM_PauseOtherVMs()
    {
        VMO_System vmo = {(VM *)malloc(3 * sizeof(VM)), 3};
        vmo.vms[0].id = 1;
        vmo.vms[0].state = VM_STATE_RUNNING;
        vmo.vms[1].id = 2;
        vmo.vms[1].state = VM_STATE_RUNNING;
        vmo.vms[2].id = 3;
        vmo.vms[2].state = VM_STATE_STOPPED;
        int result = start_vm(&vmo, 3);
        TS_ASSERT_EQUALS(result, 0);
        TS_ASSERT_EQUALS(vmo.vms[0].state, VM_STATE_PAUSED);
        TS_ASSERT_EQUALS(vmo.vms[1].state, VM_STATE_PAUSED);
        TS_ASSERT_EQUALS(vmo.vms[2].state, VM_STATE_RUNNING);
        free(vmo.vms);
    }
    ///////////////////////////////////////////////////////////////////

    void testStopVm_NullVmoSystem()
    {
        VMO_System vmo = {NULL, 0};
        int result = stop_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -2);
    }

    void testStopVm_NoVms()
    {
        VMO_System vmo = {NULL, 0};
        int result = stop_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -2);
    }

    void testStopVm_VmNotFound()
    {
        VM vms[3] = {{1, "vm1", VM_STATE_RUNNING}, {2, "vm2", VM_STATE_STOPPED}, {3, "vm3", VM_STATE_PAUSED}};
        VMO_System vmo = {vms, 3};
        int result = stop_vm(&vmo, 4);
        TS_ASSERT_EQUALS(result, -4);
    }
    ///////////////////////////////////////////////////////////////////

    void testStoppedVM()
    {
        VMO_System vmo = init_vmo_system(1);
        VM vm = {0, "test", VM_STATE_STOPPED};
        vmo.vms[0] = vm;
        VM_State result = get_vm_state(&vmo, 0);
        TS_ASSERT_EQUALS(result, VM_STATE_STOPPED);
    }

    void testRunningVM()
    {
        VMO_System vmo = init_vmo_system(1);
        VM vm = {0, "test", VM_STATE_RUNNING};
        vmo.vms[0] = vm;
        VM_State result = get_vm_state(&vmo, 0);
        TS_ASSERT_EQUALS(result, VM_STATE_RUNNING);
    }

    void testPausedVM()
    {
        VMO_System vmo = init_vmo_system(1);
        VM vm = {0, "test", VM_STATE_PAUSED};
        vmo.vms[0] = vm;
        VM_State result = get_vm_state(&vmo, 0);
        TS_ASSERT_EQUALS(result, VM_STATE_PAUSED);
    }
};