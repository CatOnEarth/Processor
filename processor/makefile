path_bin=bin
path_exe=exe
name_cpu_exe=cpu.exe
name_cpu_sfml_exe=cpu_sfml.exe
name_asm_exe=asm.exe
name_disasm_exe=disasm.exe

path_text_lib=./text_lib
path_disassembler=./disassembler
path_assembler=./assembler
path_base_lib=./base_lib
path_cpu=./cpu
path_stack_lib=$(path_cpu)/stack_lib
path_graphics=$(path_cpu)/graphics
path_logger=./logger

LDFLAGS = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
MAINFLAGS = -Wall -Wextra -Weffc++ -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Waggressive-loop-optimizations -Walloc-zero -Walloca -Walloca-larger-than=8192 -Warray-bounds -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wdangling-else -Wduplicated-branches -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Winline -Wlarger-than=8192 -Wvla-larger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wrestrict -Wshadow -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wstringop-overflow=4 -Wsuggest-attribute=noreturn -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wnarrowing -Wno-old-style-cast -Wvarargs -fcheck-new -fsized-deallocation -fstack-check -fstrict-overflow -flto-odr-type-merging

cpu_sfml: $(path_bin)/main_cpu.o $(path_bin)/cpu.o $(path_bin)/graphics.o $(path_bin)/stack_lib.o $(path_bin)/logger.o
	g++ $(path_bin)/main_cpu.o $(path_bin)/cpu.o $(path_bin)/graphics.o $(path_bin)/stack_lib.o $(path_bin)/logger.o $(LDFLAGS) -o $(path_exe)/$(name_cpu_sfml_exe) 

cpu: $(path_bin)/main_cpu.o $(path_bin)/cpu.o $(path_bin)/graphics.o $(path_bin)/stack_lib.o $(path_bin)/base_lib.o	$(path_bin)/logger.o
	g++ $(path_bin)/main_cpu.o $(path_bin)/cpu.o $(path_bin)/graphics.o $(path_bin)/stack_lib.o $(path_bin)/base_lib.o $(path_bin)/logger.o -o $(path_exe)/$(name_cpu_exe) 

asm: $(path_bin)/main_assembler.o $(path_bin)/assembler.o $(path_bin)/text_lib.o $(path_bin)/base_lib.o $(path_bin)/logger.o
	g++ $(path_bin)/main_assembler.o $(path_bin)/assembler.o $(path_bin)/text_lib.o $(path_bin)/base_lib.o $(path_bin)/logger.o -o $(path_exe)/$(name_asm_exe) 

disasm: $(path_bin)/main_disassembler.o $(path_bin)/disassembler.o $(path_bin)/base_lib.o $(path_bin)/logger.o
	g++ $(path_bin)/main_disassembler.o $(path_bin)/disassembler.o $(path_bin)/base_lib.o $(path_bin)/logger.o -o $(path_exe)/$(name_disasm_exe) 

$(path_bin)/main_cpu.o: $(path_cpu)/main_cpu.cpp
	g++ -c $(MAINFLAGS) $(path_cpu)/main_cpu.cpp -o $(path_bin)/main_cpu.o

$(path_bin)/main_assembler.o: $(path_assembler)/main_assembler.cpp
	g++ -c $(MAINFLAGS) $(path_assembler)/main_assembler.cpp -o $(path_bin)/main_assembler.o

$(path_bin)/main_disassembler.o: $(path_disassembler)/main_disassembler.cpp
	g++ -c $(MAINFLAGS) $(path_disassembler)/main_disassembler.cpp -o $(path_bin)/main_disassembler.o

$(path_bin)/disassembler.o: $(path_disassembler)/disassembler.cpp
	g++ -c $(MAINFLAGS) $(path_disassembler)/disassembler.cpp -o $(path_bin)/disassembler.o

$(path_bin)/cpu.o: $(path_cpu)/cpu.cpp
	g++ -c $(MAINFLAGS) $(path_cpu)/cpu.cpp -o $(path_bin)/cpu.o -o $(path_bin)/cpu.o

$(path_bin)/assembler.o: $(path_assembler)/assembler.cpp
	g++ -c $(MAINFLAGS) $(path_assembler)/assembler.cpp -o $(path_bin)/assembler.o

$(path_bin)/stack_lib.o: $(path_stack_lib)/stack_lib.cpp
	g++ -c $(MAINFLAGS) $(path_stack_lib)/stack_lib.cpp -o $(path_bin)/stack_lib.o

$(path_bin)/text_lib.o: $(path_text_lib)/text_lib.cpp
	g++ -c $(MAINFLAGS) $(path_text_lib)/text_lib.cpp -o $(path_bin)/text_lib.o

$(path_bin)/base_lib.o: $(path_base_lib)/base_lib.cpp
	g++ -c $(MAINFLAGS) $(path_base_lib)/base_lib.cpp -o $(path_bin)/base_lib.o

$(path_bin)/graphics.o: $(path_graphics)/graphics.cpp
	g++ -c $(MAINFLAGS) $(path_graphics)/graphics.cpp -o $(path_bin)/graphics.o

$(path_bin)/logger.o: $(path_logger)/logger.cpp
	g++ -c $(MAINFLAGS) $(path_logger)/logger.cpp -o $(path_bin)/logger.o
