#include "bitmap.h"

/*
The init_vmo_system function initializes a virtual machine orchestration (VMO) system by creating
an array of virtual machines (VMs) and initializing each VM in the array with an ID, name, and state (stopped).
The function returns a VMO_System struct containing the array of VMs and the number of VMs in the system.
If the number of VMs is negative or memory allocation fails, the function returns an empty VMO_System struct.
*/
VMO_System init_vmo_system(int num_vms)
{
}

/*
The function add_vm adds a new virtual machine (VM) to the Virtual Machine Orchestration (VMO) system.
It takes a pointer to the VMO system and a string for the name of the new VM as input. It checks the
validity of the input parameters, creates a new VM struct with an incremented ID and a stopped state,
and then allocates memory for the new VM and copies its data into the VMO system.
Finally, it returns the ID of the newly created VM, or -1 if there was an error.
*/
int add_vm(VMO_System *vmo, char *name)
{
}

/*
The function remove_vm removes a virtual machine with a specified ID from the Virtual Machine Orchestration (VMO) system.
If the VMO system or the array of VMs is invalid, the function returns -1. If the VM with the specified ID is not found
in the array, it returns -2. If the VM is successfully removed, the function shifts all VMs after the deleted VM one position to the left,
clears the last VM in the array, and decrements the number of VMs. The function returns 0 if the operation is successful.
*/
int remove_vm(VMO_System *vmo, int id)
{
}

/*
This function starts a virtual machine specified by an ID in a VMO system. If the virtual machine is already running or paused,
it returns an error code. If the virtual machine is not found in the VMO system, it also returns an error code.
If the virtual machine is started, it checks if there are other running virtual machines in the VMO system.
If there are, it pauses them and returns 0, indicating success.
*/
int start_vm(VMO_System *vmo, int id)
{
}

/*
The function stop_vm() takes a pointer to a Virtual Machine Orchestration (VMO) system and an ID of a virtual machine as input.
It then searches for the virtual machine with the given ID in the VMO system and sets its state to "stopped" if it is currently running.
The function returns 0 on success, and various error codes (-1 to -5) if the VMO system is not initialized, there are no virtual machines,
the virtual machine is not running, or it is not found in the system.
*/
int stop_vm(VMO_System *vmo, int id)
{
}

/*
This function retrieves the state of a virtual machine with the specified ID in a Virtual Machine Orchestration Management System (VMO System).
It checks if the VMO system is empty, if the ID is valid and if the virtual machine is running, paused or stopped.
If the virtual machine is null, stopped or the ID is invalid, it returns the stopped state.
*/
VM_State get_vm_state(VMO_System *vmo, int id)
{
}
