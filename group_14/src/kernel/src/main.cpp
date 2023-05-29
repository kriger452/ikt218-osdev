#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Custom files
#include "include/descriptor_tables.h"
#include "include/interrupts.h"
#include "include/common.h"
#include "include/sc_to_ascii.h"
#include "include/memory.h"
#include "include/pit.h"
#include "include/terminal.h"

// Define entry point in asm to prevent C++ mangling
extern "C"{
   void kernel_main();
}

extern uint32_t end; // This is defined in linker.ld

// Overload the new operator for single object allocation
void* operator new(size_t size) {
    return malloc(size);   // Call the C standard library function malloc() to allocate memory of the given size and return a pointer to it
}

// Overload the delete operator for single object deallocation
void operator delete(void* ptr) noexcept {
    free(ptr);             // Call the C standard library function free() to deallocate the memory pointed to by the given pointer
}

void kernel_main()
{

    // Initialize terminal and
	// and display a string.
    terminal_initialize();

    // Initialize Global Descriptor Table (GDT)
    init_gdt();

    // Initialize Interrupt Descriptor Table (IDT)
    init_idt();

    // Initialize Interrupt Requests (IRQs)
    init_irq();

    // Initialize kernel memory manager with the end of the kernel image
    init_kernel_memory(&end);
    
    // Initialize Paging
    init_paging();

    // Print memory layout
    print_memory_layout();

    // Setup PIT
    //init_pit(); 

    // Allocate some memory using kernel memory manager
    void* some_memory = malloc(12345); 
    void* memory2 = malloc(54321); 
    void* memory3 = malloc(13331);
    //char* memory4 = new char[1000]();

	// Create interrupt handlers for interrupt 3 and 4
    /*
    register_interrupt_handler(3,[](registers_t* regs, void* context){
        terminal_writestring("Interrupt 3 - OK\n");
    }, NULL);

    register_interrupt_handler(4,[](registers_t* regs, void* context){
        terminal_writestring("Interrupt 4 - OK\n");
    }, NULL);


    // Trigger interrupts 3 and 4 which should call the respective handlers
    asm volatile ("int $0x3");
    asm volatile ("int $0x4");*/

    // Disable interrupts temporarily
    asm volatile("sti");

    // Create an IRQ handler for IRQ1
    register_irq_handler(IRQ1, [](registers_t*, void*){
         // Read the scan code from keyboard
        unsigned char scan_code = inb(0x60);
        unsigned char ascii = scancode_to_ascii(scan_code);

        terminal_putchar(ascii);
        // Disable interrupts temporarily
        asm volatile("cli");
    }, NULL);

    // Print a message and enter an infinite loop to wait for interrupts
    // terminal_writestring("Waiting...\n");
    while(1){};
    terminal_writestring("Done!...\n");
}


