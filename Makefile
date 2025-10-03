COMPILER := g++
DED_FLAGS := -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
#MY_FLAGS := -D NDEBUG

COMPILER_FLAGS := $(DED_FLAGS) $(MY_FLAGS)

BUILD_DIR := build

libs := byteio
processor_sources := main stack executor
compiler_sources := main parser text_utils

.PHONY: build clean

build : compiler.out processor.out

processor.out : $(processor_sources:%=$(BUILD_DIR)/processor/%.o) $(libs:%=$(BUILD_DIR)/%.o)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@
	
compiler.out : $(compiler_sources:%=$(BUILD_DIR)/compiler/%.o) $(libs:%=$(BUILD_DIR)/%.o)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@

-include $(libs:%=$(BUILD_DIR)/%.d)
-include $(processor_sources:%=$(BUILD_DIR)/processor/%.d)
-include $(compiler_sources:%=$(BUILD_DIR)/compiler/%.d)

$(BUILD_DIR)/%.o : %.cpp
	mkdir -p $(@D)
	$(COMPILER) $(COMPILER_FLAGS) -MP -MMD $< -c -o $@


clean :
	-rm -r $(BUILD_DIR)
	-rm *.out
