#include <iostream>
#include <cstring>
#include <fstream>
#include <bitset>
#include <cstdarg>
#include <cstdio>
#include "cpu.hh"
#include <vector>

void CPU::log(const char* input, ...) {
	if (this->debug) {
		va_list args;
		va_start(args, input);
		vprintf(input, args);
		va_end(args);
	}
}

void CPU::init(Display& d) {
	display = &d;
	pc      = 0x200;
	opcode  = 0x0;
	I       = 0x0;
	sp      = 0x0;

    memset(&d.mem,  0, sizeof(d.mem));
    memset(&V,      0, sizeof(V));
    memset(&stack,  0, sizeof(stack));
    memset(&memory, 0, sizeof(memory));

	tables();

	const std::vector<uint8_t> m_fontset = {
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int i = 0; i < m_fontset.size(); ++i) {
		memory[0x50 + i] = m_fontset[i];
	}

	delay_timer = 0;
	sound_timer = 0;
}

void CPU::tables() {
	// todo tomorrow :)
}

void CPU::toggle_pause() {
	pause_execution ^= true;
}

bool CPU::load_game(const std::string& name) {
	std::ifstream file(name, std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		std::streampos size = file.tellg();
		char buffer[4096];

		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (int i = 0; i < size; ++i) {
			memory[0x200 + i] = buffer[i];
		}

		return true;
	}
	return false;
}

void CPU::update_timers() {
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0 && (sound_timer % 10) == 0) {
		log("BEEP!\n");
		--sound_timer;
	}
}

void CPU::cycle() {
	cycles++;
	opcode = memory[pc] << 8 | memory[pc + 1];

	uint16_t  vx  = (opcode & 0x0f00) >> 8;
	uint16_t  vy  = (opcode & 0x00f0) >> 4;
	uint16_t  nnn = (opcode & 0x0fff);
	uint8_t   nn  = (opcode & 0x00ff);
	uint8_t   n   = (opcode & 0x000f);
	bool      waiting = false, skip = false;

	if (trace && (opcode & 0xFFFF) != 0x0000) {
		printf("%d: ", cycles);
		printf("m_pc: %.4X / m_opcode: %.4X / m_sp: %.2X ", pc, opcode, sp);
		printf("/ index: %.4X ", I);
		printf("%s", "/ registers: ");
		for (int i = 0; i < 15; i++)
			printf("%.2X ", V[i]);
		printf("\n");
	}

	// TODO function pointers
	// they're already defined in the header, I just need to make use of them rather than this atrocity
	switch (opcode & 0xF000) {
		case 0x0000: this->table0(opcode);                 break;
		case 0x1000: this->jump(nnn);                      break;
		case 0x2000: this->call_subroutine(nnn);           break;
		case 0x3000: this->skip_if_equal_nn(skip, vx, nn); break;
		case 0x4000: this->skip_if_not_equal(vx, nn);      break;
		case 0x5000: this->skip_if_equal_vy(skip, vx, vy); break;
		case 0x6000: this->set_vx(vx, nn);                 break;
		case 0x7000: this->add(vx, nn);                    break;
		case 0x8000: this->table8(opcode, vx, vy);         break;
		case 0x9000: this->skip_if_not_equal(vx, vy);      break;
		case 0xA000: this->set_index(nnn);                 break;
		case 0xB000: this->jump_to_offset(nnn);            break;
		case 0xC000: this->vx_rand(vx, nn);                break;
		case 0xD000: this->draw(vx, vy, n);                break;
		case 0xE000: this->tableE(skip, opcode, vx);       break;
		case 0xF000: this->tableF(waiting, opcode, vx);    break;
    }

	update_timers();
	update_pc(skip, waiting);
}

void CPU::update_pc(bool skip = false, bool waiting = false) {
	if (waiting) {
		pc -= 2;
		return;
	}

	if (skip) {
		pc += 4;
		return;
	}

	pc += 2;
}

void CPU::table0(uint16_t opcode) {
	uint16_t mask = opcode & 0x00ff;

	if (mask == 0x00e0) {
		this->clear_display();
		return;
	}

	if (mask == 0x00ee) {
		this->return_from_subroutine();
		return;
	}

	std::cerr << "Unknown opcode: " << opcode;
	exit(3);
}

void CPU::table8(uint16_t opcode, uint16_t vx, uint16_t vy) {
	uint16_t mask = opcode & 0x000f;

	if (mask == 0x0000) {
		this->set_vx_vy(vx, vy);
		return;
	}

	if (mask == 0x0001) {
		this->vx_or(vx, vy);
		return;
	}

	if (mask == 0x0002) {
		this->vx_and(vx, vy);
		return;
	}

	if (mask == 0x0003) {
		this->vx_xor(vx, vy);
		return;
	}

	if (mask == 0x0004) {
		this->add_with_carry(vx, vy);
		return;
	}

	if (mask == 0x0005) {
		this->sub_with_borrow(vx, vy);
		return;
	}

	if (mask == 0x0006) {
		this->shift_right(vx);
		return;
	}

	if (mask == 0x0007) {
		this->reverse_sub(vx, vy);
		return;
	}

	if (mask == 0x000e) {
		this->shift_right(vx);
		return;
	}

	std::cerr << "Unknown opcode: " << std::hex << opcode;
	exit(3);
}

void CPU::tableE(bool& f, uint16_t opcode, uint16_t vx) {
	uint16_t mask = opcode & 0x00ff;

	if (mask == 0x009e) {
		this->skip_if_pressed(f, vx);
		return;
	}

	if (mask == 0x00a1) {
		this->skip_if_not_pressed(f, vx);
		return;
	}

	std::cerr << "Unknown opcode: " << std::hex << opcode;
	exit(3);
}

void CPU::tableF(bool& f, uint16_t opcode, uint16_t vx) {
	uint16_t mask = opcode & 0x00ff;

	if (mask == 0x0007) {
		this->set_vx_to_delay(vx);
		return;
	}
	
	if (mask == 0x000a) {
		this->wait_for_key(waiting, vx);
		return;
	}

	if (mask == 0x0015) {
		this->set_delay(vx);
		return;
	}

	if (mask == 0x0018) {
		this->set_sound(vx);
		return;
	}

	if (mask == 0x001e) {
		this->add_index(vx);
		return;
	}

	if (mask == 0x0029) {
		this->set_index(vx);
		return;
	}

	if (mask == 0x0033) {
		this->vx_binary(vx);
		return;
	}

	if (mask == 0x0055) {
		this->store(vx);
		return;
	}

	if (mask == 0x0065) {
		this->read(vx);
		return;
	}

	std::cerr << "Unknown opcode: " << std::hex << opcode;
	exit(3);
}

void CPU::clear_display() {
	display->clear();
	display->m_draw_flag = true;
	update_pc();
}

void CPU::return_from_subroutine() {
	pc = stack[sp];
	sp--;
	update_pc();
}

void CPU::jump(uint16_t nnn) {
	pc = nnn;
}

void CPU::call_subroutine(uint16_t nnn) {
	stack[sp] = pc;
	sp++;
	pc = nnn;
}

void CPU::skip_if_equal_nn(bool& f, uint16_t vx, uint8_t nn) {
	f = V[vx] == nn;
	update_pc(f);
}

void CPU::skip_if_not_equal(bool& f, uint16_t vx, uint8_t nn) {
	if (V[vx] != nn)
		pc += 4;
	else
		update_pc();
}

void CPU::skip_if_equal_vy(bool& f, uint16_t vx, uint16_t vy) {
	if (V[vx] == V[vy])
		pc += 4;
	else 
		update_pc();
}

void CPU::set_vx(uint16_t vx, uint8_t nn) {
	log("m_opcode: 0x%X\n", opcode);
	log("Vx:       %d (0x%X)\n", vx, vx);
	log("Vx:       %s\n", std::bitset<16>(vx).to_string().c_str());
	log("nn:       %d (0x%X)\n", nn, nn);
	log("nn:       %s\n", std::bitset<16>(nn).to_string().c_str());
	V[vx] = nn;
	update_pc();
}

void CPU::add(uint16_t vx, uint8_t nn) {
	V[vx] += nn;
	update_pc();
}

void CPU::set_vx_vy(uint16_t vx, uint16_t vy) {
	V[vx] = V[vy];
	update_pc();
}

void CPU::vx_or(uint16_t vx, uint16_t vy) {
	V[vx] |= V[vy];
	update_pc();
}

void CPU::vx_and(uint16_t vx, uint16_t vy) {
	V[vx] &= V[vy];
	update_pc();
}

void CPU::vx_xor(uint16_t vx, uint16_t vy) {
	V[vx] ^= V[vy];
	update_pc();
}

void CPU::add_with_carry(uint16_t vx, uint16_t vy) {
	uint16_t result = vx + vy;
	V[0xf] = result > 0xffff;
	V[vx] = result & 0xff;
	update_pc();
}

void CPU::sub_with_borrow(uint16_t vx, uint16_t vy) {
	V[0xf] = V[vx] > V[vy];
	V[vx] -= vy;
	update_pc();
}

void CPU::shift_right(uint16_t vx) {
	V[0xf] = (V[vx] & 0x1);
	V[vx] >>= 1;
	update_pc();
}

void CPU::reverse_sub(uint16_t vx, uint16_t vy) {
	V[0xf] = V[vy] > V[vx];
	V[vx] = vy - vx;
	update_pc();
}

void CPU::shift_left(uint16_t vx) {
	V[0xf] = vx >> 7;
	V[vx] <<= 1;
	update_pc();
}

void CPU::skip_if_not_equal(uint16_t vx, uint16_t vy) {
	if (V[vx] != V[vy])
		pc += 4;
	else
		update_pc();
}

void CPU::set_index(uint16_t nnn) {
	I = nnn;
	update_pc();
}

void CPU::jump_to_offset(uint16_t nnn) {
	pc = nnn + V[0];
	update_pc();
}

void CPU::vx_rand(uint16_t vx, uint8_t nn) {
	V[vx] = (rand() % 256) & nn;
	update_pc();
}

void CPU::draw(uint16_t vx, uint16_t vy, uint8_t n) {
	// Draw a sprite at coordinate VX, VY that has a width of 8 pixels and a height of N pixels
	// Each row is bit-coded starting from I
	// I does not change 
	// VF is set to 1 if any bits are flipped off, 0 otherwise
	uint8_t height = n;
	uint8_t xPos = V[vx] % display->m_video_width;
	uint8_t yPos = V[vy] % display->m_video_height;
	log("m_opcode: 0x%X\n", opcode);
	log("Drawing at x: %d, y: %d\n", xPos, yPos);
	log("Drawing height: %d\n", height);
	log("Vx: %d\n", vx);
	log("Vy: %d\n", vy);

	V[0xf] = display->set(&memory[I], height, xPos, yPos);
	display->m_draw_flag = true;
	update_pc();
}

void CPU::skip_if_pressed(bool& f, uint16_t vx) {
	// Skip instruction if VX is pressed
	if (keypad[vx])
		pc += 4;
	else
		update_pc();
}

void CPU::skip_if_not_pressed(bool& f, uint16_t vx) {
	// Skip instruction if VX is not pressed
	if (!keypad[vx])
		pc += 4;
	else
		update_pc();
}

void CPU::set_vx_to_delay(uint16_t vx) {
	// Set VX to the value of the delay time
	V[opcode & 0x0F00 >> 8] = delay_timer;
	update_pc();
}

void CPU::wait_for_key(bool& f, uint16_t vx) {
	// Wait for a key press, then store it in VX
	waiting = true;
	for (int i = 0; i < 0xf; i++) {
		if (keypad[i]) {
			V[vx] = i;
			f = false;
		}
	}
	if (waiting)
		pc -= 2;
	else 
		update_pc();
}

void CPU::set_delay(uint16_t vx) {
	// Set Delay Timer to the value stored in VX
	delay_timer = V[vx];
	update_pc();
}

void CPU::set_sound(uint16_t vx) {
	// Set Sound Timer to the value stored in VX
	sound_timer = V[vx];
	update_pc();
}

void CPU::add_index(uint16_t vx) {
	// Add VX to Index
	I += V[vx];
	update_pc();
}

void CPU::set_index(uint16_t vx) {
	// Set Index to the location of the sprite for VX
	I = V[vx];
	update_pc();
}

void CPU::vx_binary(uint16_t vx) {
	// Store the Binary represenation of VX at Index+[0,1,2]
	memory[I]   = (vx / 100) % 10;
	memory[I+1] = (vx / 10) % 10;
	memory[I+2] = vx % 10;
	update_pc();
}

void CPU::store(uint16_t vx) {
	// Store registers V0 through Vx in m_ram starting at location m_index.
	// The interpreter copies the values of registers V0 through Vx into m_ram, starting at the address in m_index.
	for (int i = 0; i <= vx; ++i) {
		memory[I + i] = V[i];
	}
	update_pc();
}

void CPU::read(uint16_t vx) {
	// Read registers V0 through Vx from m_ram starting at location m_index.
	// The interpreter reads values from m_ram starting at location m_index into registers V0 through Vx.
	for (int i = 0; i <= vx; ++i) {
		V[i] = memory[I + i];
	}
	update_pc();
}
