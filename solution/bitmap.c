#include "bitmap.h"

/*
The init_vmo_system function initializes a virtual machine orchestration (VMO) system by creating
an array of virtual machines (VMs) and initializing each VM in the array with an ID, name, and state (stopped).
The function returns a VMO_System struct containing the array of VMs and the number of VMs in the system.
If the number of VMs is negative or memory allocation fails, the function returns an empty VMO_System struct.
*/
VMO_System init_vmo_system(int num_vms)
{
    // Check if num_vms is non-negative
    if (num_vms < 0)
    {
        VMO_System empty_system = {NULL, 0};
        return empty_system;
    }

    // Allocate memory for the array of VMs
    VM *vms = (VM *)malloc(num_vms * sizeof(VM));
    if (vms == NULL)
    {
        VMO_System empty_system = {NULL, 0};
        return empty_system;
    }

    // Initialize each VM in the array
    for (int i = 0; i < num_vms; i++)
    {
        char id_str[10];
        sprintf(id_str, "%d", i); // Convert the integer i to a string
        vms[i].id = i;
        strcpy(vms[i].name, "VM");
        strcat(vms[i].name, id_str); // Concatenate the string to the name
        vms[i].state = VM_STATE_STOPPED;
    }

    // Initialize the VMO system with the array of VMs
    VMO_System vmo_system = {vms, num_vms};
    return vmo_system;
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
    if (vmo == NULL || name == NULL || vmo->num_vms < 0)
    {
        // Invalid input parameters
        return -1;
    }

    if (strlen(name) > 49)
    {
        // Name is too long
        return -1;
    }

    // Create a new VM struct and initialize its fields
    VM new_vm;
    new_vm.id = vmo->num_vms + 1;
    strcpy(new_vm.name, name);
    new_vm.state = VM_STATE_STOPPED;

    // Allocate memory for the new VM and copy its data into the VMO system
    VM *new_vms = (VM *)realloc(vmo->vms, (vmo->num_vms + 1) * sizeof(VM));
    if (new_vms == NULL)
    {
        // Failed to allocate memory for the new VM
        return -1;
    }
    new_vms[vmo->num_vms] = new_vm;
    vmo->vms = new_vms;
    vmo->num_vms++;

    return new_vm.id;
}

/*
The function remove_vm removes a virtual machine with a specified ID from the Virtual Machine Orchestration (VMO) system.
If the VMO system or the array of VMs is invalid, the function returns -1. If the VM with the specified ID is not found
in the array, it returns -2. If the VM is successfully removed, the function shifts all VMs after the deleted VM one position to the left,
clears the last VM in the array, and decrements the number of VMs. The function returns 0 if the operation is successful.
*/
int remove_vm(VMO_System *vmo, int id)
{
    if (vmo == NULL || vmo->vms == NULL || vmo->num_vms <= 0)
    {
        // Error: invalid VMO system
        return -1;
    }

    // Find the VM with the given ID
    int vm_index = -1;
    for (int i = 0; i < vmo->num_vms; i++)
    {
        if (vmo->vms[i].id == id)
        {
            vm_index = i;
            break;
        }
    }

    if (vm_index < 0)
    {
        // Error: VM not found
        return -2;
    }

    // Shift all VMs after the deleted VM one position to the left
    for (int i = vm_index; i < vmo->num_vms - 1; i++)
    {
        vmo->vms[i] = vmo->vms[i + 1];
    }

    // Clear the last VM in the array and decrement the number of VMs
    memset(&vmo->vms[vmo->num_vms - 1], 0, sizeof(VM));
    vmo->num_vms--;

    return 0;
}

/*
This function starts a virtual machine specified by an ID in a VMO system. If the virtual machine is already running or paused,
it returns an error code. If the virtual machine is not found in the VMO system, it also returns an error code.
If the virtual machine is started, it checks if there are other running virtual machines in the VMO system.
If there are, it pauses them and returns 0, indicating success.
*/
int start_vm(VMO_System *vmo, int id)
{
    if (vmo == NULL || vmo->vms == NULL)
    {
        // VMO system or virtual machine array is not initialized
        return -1;
    }
    int vm_index = -1;
    for (int i = 0; i < vmo->num_vms; i++)
    {
        if (vmo->vms[i].id == id)
        {
            vm_index = i;
            break;
        }
    }
    if (vm_index == -1)
    {
        // Virtual machine with specified ID not found in VMO system
        return -2;
    }
    VM_State current_state = vmo->vms[vm_index].state;
    if (current_state == VM_STATE_RUNNING)
    {
        // Virtual machine is already running
        return -3;
    }
    else if (current_state == VM_STATE_PAUSED)
    {
        // Virtual machine is paused, resume execution
        vmo->vms[vm_index].state = VM_STATE_RUNNING;
        return 0;
    }
    // Start the virtual machine
    vmo->vms[vm_index].state = VM_STATE_RUNNING;
    // Check if any other virtual machines are running
    int other_running_vm_count = 0;
    for (int i = 0; i < vmo->num_vms; i++)
    {
        if (i != vm_index && vmo->vms[i].state == VM_STATE_RUNNING)
        {
            other_running_vm_count++;
        }
    }
    if (other_running_vm_count > 0)
    {
        // Pause other running virtual machines
        for (int i = 0; i < vmo->num_vms; i++)
        {
            if (i != vm_index && vmo->vms[i].state == VM_STATE_RUNNING)
            {
                vmo->vms[i].state = VM_STATE_PAUSED;
            }
        }
    }
    return 0;
}

/*
The function stop_vm() takes a pointer to a Virtual Machine Orchestration (VMO) system and an ID of a virtual machine as input.
It then searches for the virtual machine with the given ID in the VMO system and sets its state to "stopped" if it is currently running.
The function returns 0 on success, and various error codes (-1 to -5) if the VMO system is not initialized, there are no virtual machines,
the virtual machine is not running, or it is not found in the system.
*/
int stop_vm(VMO_System *vmo, int id)
{
    if (!vmo)
    {
        // VMO system not initialized
        return -1;
    }
    if (!vmo->vms || vmo->num_vms == 0)
    {
        // No virtual machines in the VMO system
        return -2;
    }
    int found = 0;
    for (int i = 0; i < vmo->num_vms; i++)
    {
        if (vmo->vms[i].id == id)
        {
            found = 1;
            if (vmo->vms[i].state != VM_STATE_RUNNING)
            {
                // Virtual machine is not running
                return -3;
            }
            vmo->vms[i].state = VM_STATE_STOPPED;
            return 0;
        }
    }
    if (!found)
    {
        // Virtual machine not found in the VMO system
        return -4;
    }
    return -5;
}

/*
This function retrieves the state of a virtual machine with the specified ID in a Virtual Machine Orchestration Management System (VMO System).
It checks if the VMO system is empty, if the ID is valid and if the virtual machine is running, paused or stopped.
If the virtual machine is null, stopped or the ID is invalid, it returns the stopped state.
*/
VM_State get_vm_state(VMO_System *vmo, int id)
{
    // Check if the VMO system is empty or null
    if (vmo == NULL || vmo->vms == NULL || vmo->num_vms == 0)
    {
        return VM_STATE_STOPPED;
    }
    // Check if the ID is valid
    if (id < 0 || id >= vmo->num_vms)
    {
        return VM_STATE_STOPPED;
    }
    // Get the virtual machine with the specified ID
    VM vm = vmo->vms[id];
    // Check if the virtual machine is null
    if (&vm == NULL)
    {
        return VM_STATE_STOPPED;
    }
    // Check if the virtual machine is running
    if (vm.state == VM_STATE_RUNNING)
    {
        return VM_STATE_RUNNING;
    }
    // Check if the virtual machine is paused
    if (vm.state == VM_STATE_PAUSED)
    {
        return VM_STATE_PAUSED;
    }
    // Otherwise, the virtual machine must be stopped
    return VM_STATE_STOPPED;
}
