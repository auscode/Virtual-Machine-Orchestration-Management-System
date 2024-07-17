#include <cxxtest/TestSuite.h>
#include "../src/bitmap.h"

class SampleTestSuite : public CxxTest::TestSuite
{
public:
    void testInitVMOSystem_MemoryAllocationError()
    {
        // Ensure that the function returns an empty system if memory allocation fails
        VMO_System system = init_vmo_system(1000000000);
        TS_ASSERT(system.vms == NULL);
        TS_ASSERT_EQUALS(system.num_vms, 0);
    }
    void testInitVMOSystem_ValidNumVMs()
    {
        VMO_System system = init_vmo_system(3);
        TS_ASSERT_EQUALS(system.num_vms, 3);
        for (int i = 0; i < system.num_vms; i++)
        {
            TS_ASSERT_EQUALS(system.vms[i].id, i);
            TS_ASSERT_EQUALS(strcmp(system.vms[i].name, "VM") == 0, false);
            TS_ASSERT_EQUALS(system.vms[i].state, VM_STATE_STOPPED);
        }
    }

    // Test initialization of VMO system with an invalid number of virtual machines
    void testInitVMOSystem_InvalidNumVMs()
    {
        VMO_System system = init_vmo_system(-1);
        TS_ASSERT_EQUALS(system.num_vms, 0);
        TS_ASSERT_EQUALS(system.vms, (VM *)NULL);
    }
    void testInitVMOSystem_NegativeNumVMs()
    {
        VMO_System system = init_vmo_system(-1);
        TS_ASSERT(system.vms == NULL);
        TS_ASSERT_EQUALS(system.num_vms, 0);
    }
    ////////////////////////////////////////////////////////////////////

    void testAddVm_NullPointer()
    {
        VMO_System vmo = init_vmo_system(0);
        char name[] = "vm1";
        int result = add_vm(NULL, name);
        TS_ASSERT_EQUALS(result, -1);
        TS_ASSERT_EQUALS(vmo.num_vms, 0);
    }

    void test_add_vm_with_negative_num_vms()
    {
        VMO_System vmo = init_vmo_system(-1);
        char name[] = "VM1";
        int result = add_vm(&vmo, name);
        TS_ASSERT_EQUALS(result, 1);
        TS_ASSERT_EQUALS(vmo.num_vms, 1);
    }
    void test_add_vm_with_long_name()
    {
        VMO_System vmo = init_vmo_system(0);
        char name[] = "this_name_is_longer_than_50_characters_this_name_is_longer_than_50_characters_this_name_is_longer_than_50_characters";
        int result = add_vm(&vmo, name);
        TS_ASSERT_EQUALS(result, -1);
        TS_ASSERT_EQUALS(vmo.num_vms, 0);
    }
    ///////////////////////////////////////////////////////////////////
    void testRemoveVM_NullVMArray()
    {
        VMO_System vmo = {NULL, 5};
        int result = remove_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -1);
    }

    void testRemoveVM_EmptyVMOSystem()
    {
        VMO_System vmo = {(VM *)malloc(0), 0};
        int result = remove_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -1);
        free(vmo.vms);
    }

    void testRemoveVM_MiddleVM()
    {
        VMO_System vmo = {(VM *)malloc(3 * sizeof(VM)), 3};
        vmo.vms[0] = (VM){1, "VM1", VM_STATE_RUNNING};
        vmo.vms[1] = (VM){2, "VM2", VM_STATE_STOPPED};
        vmo.vms[2] = (VM){3, "VM3", VM_STATE_PAUSED};
        int result = remove_vm(&vmo, 2);
        TS_ASSERT_EQUALS(result, 0);
        TS_ASSERT_EQUALS(vmo.num_vms, 2);
        TS_ASSERT_EQUALS(vmo.vms[0].id, 1);
        TS_ASSERT_EQUALS(vmo.vms[1].id, 3);
        free(vmo.vms);
    }
    void testRemoveVM_VMNotFound()
    {
        VMO_System vmo = {(VM *)malloc(2 * sizeof(VM)), 2};
        vmo.vms[0] = (VM){1, "VM1", VM_STATE_RUNNING};
        vmo.vms[1] = (VM){2, "VM2", VM_STATE_STOPPED};
        int result = remove_vm(&vmo, 3);
        TS_ASSERT_EQUALS(result, -2);
        TS_ASSERT_EQUALS(vmo.num_vms, 2);
        free(vmo.vms);
    }
    ////////////////////////////////////////////////////////////

    void testStartVM_UninitializedVMOSystem()
    {
        VMO_System vmo = {NULL, 5};
        int result = start_vm(&vmo, 2);
        TS_ASSERT_EQUALS(result, -1);
    }

    void testStartVM_InvalidID()
    {
        VMO_System vmo = {(VM *)malloc(2 * sizeof(VM)), 2};
        vmo.vms[0].id = 1;
        vmo.vms[1].id = 3;
        int result = start_vm(&vmo, 2);
        TS_ASSERT_EQUALS(result, -2);
        free(vmo.vms);
    }
    void testStartVM_PausedVM()
    {
        VMO_System vmo = {(VM *)malloc(2 * sizeof(VM)), 2};
        vmo.vms[0].id = 1;
        vmo.vms[0].state = VM_STATE_PAUSED;
        int result = start_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, 0);
        TS_ASSERT_EQUALS(vmo.vms[0].state, VM_STATE_RUNNING);
        free(vmo.vms);
    }

    //////////////////////////////////////////////////////////

    void testStopVm_VmNotRunning()
    {
        VM vms[2] = {{1, "vm1", VM_STATE_PAUSED}, {2, "vm2", VM_STATE_STOPPED}};
        VMO_System vmo = {vms, 2};
        int result = stop_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -3);
    }

    void testStopVm_VmStopped()
    {
        VM vms[1] = {{1, "vm1", VM_STATE_STOPPED}};
        VMO_System vmo = {vms, 1};
        int result = stop_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, -3);
        TS_ASSERT_EQUALS(vms[0].state, VM_STATE_STOPPED);
    }

    void testStopVm_VmRunning()
    {
        VM vms[1] = {{1, "vm1", VM_STATE_RUNNING}};
        VMO_System vmo = {vms, 1};

        int result = stop_vm(&vmo, 1);
        TS_ASSERT_EQUALS(result, 0);
        TS_ASSERT_EQUALS(vms[0].state, VM_STATE_STOPPED);
    }

    ///////////////////////////////////////////////////////////////////

    void testEmptySystem()
    {
        VMO_System vmo = init_vmo_system(0);
        VM_State result = get_vm_state(&vmo, 0);
        TS_ASSERT_EQUALS(result, VM_STATE_STOPPED);
    }

    void testInvalidID()
    {
        VMO_System vmo = init_vmo_system(2);
        VM_State result = get_vm_state(&vmo, 3);
        TS_ASSERT_EQUALS(result, VM_STATE_STOPPED);
    }

    void testNullVMO()
    {
        VM_State result = get_vm_state(NULL, 0);
        TS_ASSERT_EQUALS(result, VM_STATE_STOPPED);
    }

};