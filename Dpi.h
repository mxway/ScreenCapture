#ifndef DPI_H
#define DPI_H

enum ProcessDpiAwareness{
    Processs_Dpi_Unware = 0,
    Porcess_System_Dpi_Aware = 1,
    Process_Per_Monitor_Dpi_Aware = 2,
};

class Dpi {
public:
    static void SetDpiAwareness(ProcessDpiAwareness dpiAwareness = Processs_Dpi_Unware);
};

#endif //DPI_H
