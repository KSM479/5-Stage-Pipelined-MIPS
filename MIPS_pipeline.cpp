/*
NAME                -           KALPAN MEHTA
NET ID              -           KSM469
UNIVERSITY ID       -           N12206794
CONTACT NO.         -           +1 (201) 936-7028
EMAIL               -           ksm469@nyu.edu


THIS IS THE COMPLETE CODE FOR 5 STAGE MIPS PIPELINE.
SUPPORTED INSTRUCTIONS ARE : ADDU , SUBU , BEQ , LW , SW, HALT (FF - JUST TO END THE EXECUTION)
YOU WILL NEED A DMEM FILE & IMEM FILE TO RUN THIS CODE.
THE CODE WILL GENERATE STATERESULT & DMEM RESULT & RFRESULT FILE 

*/
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000

struct IFStruct {
    bitset<32>  PC;
    bool        nop ;
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop ;
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type ;
    bool        rd_mem;
    bool        wrt_mem;
    bool        alu_op;
    bool        wrt_enable;
    bool        nop;
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem;
    bool        wrt_enable;
    bool        nop;
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable ;
    bool        nop;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public:
        bitset<32> Reg_data;
     	RF()
    	{
			Registers.resize(32);
			Registers[0] = bitset<32> (0);
        }

        bitset<32> readRF(bitset<5> Reg_addr)
        {
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }

        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }

		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();
		}

	private:
		vector<bitset<32> >Registers;
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {
			IMem.resize(MemSize);
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{
					IMem[i] = bitset<8>(line);
					i++;
				}
			}
            else cout<<"Unable to open file";
			imem.close();
		}

		bitset<32> readInstr(bitset<32> ReadAddress)
		{
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;
		}

    private:
        vector<bitset<8> > IMem;
};

class DataMem
{
    public:
        bitset<32> ReadData;
        DataMem()
        {
            DMem.resize(MemSize);
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();
        }

        bitset<32> readDataMem(bitset<32> Address)
        {
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;
		}

        void writeDataMem(bitset<32> Address, bitset<32> WriteData)
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));
        }

        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {
                    dmemout << DMem[j]<<endl;
                }

            }
            else cout<<"Unable to open file";
            dmemout.close();
        }

    private:
		vector<bitset<8> > DMem;
};

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl;

        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl;

        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl;
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;

        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl;
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl;
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl;
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl;
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl;
    }
    else cout<<"Unable to open file";
    printstate.close();
}

bitset<32> signextend (bitset<16> imm)
{
    string sestring;
    if (imm[15]==0){
        sestring = "0000000000000000"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    else{
        sestring = "1111111111111111"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    return (bitset<32> (sestring));

}

unsigned long shiftbits(bitset<32> inst, int start)
{
    unsigned long ulonginst;
    return ((inst.to_ulong())>>start);

}

bitset<32> BranchAddr (bitset<16> imm)
{
    string sestring1;
    if (imm[15]==0){
        sestring1 = "00000000000000"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>()+"00";
    }
    else{
        sestring1 = "11111111111111"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>()+"00";
    }
    return (bitset<32> (sestring1));

}



int main()
{
    bitset<6> opcode;
    bitset<6> funct;
    bool IsBranch;
    bitset<1> IsLoad;
    bitset<1> IsStore;
    int cycle=0;
    bitset<1> IsEq;
    bool stall=0;
    bitset<32>BranchAddress;
    bool stall1 = 0;


    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    stateStruct state, newState;
    state.IF.PC = bitset<32> (0);
    newState.IF.PC = bitset<32> (0);
    newState.IF.nop = false;
    state.IF.nop = false;
    state.ID.nop= true;
    newState.ID.nop = true;
    state.EX.is_I_type = false;
    newState.EX.is_I_type = false;
    state.EX.rd_mem= false;
    newState.EX.rd_mem = false;
    state.EX.wrt_mem = false;
    newState.EX.wrt_mem = false;
    state.EX.wrt_enable = false;
    newState.EX.wrt_enable = false;
    state.EX.nop=true;
    newState.EX.nop = true;
    state.MEM.rd_mem = false;
    newState.MEM.rd_mem = false;
    state.MEM.wrt_mem = false;
    newState.MEM.wrt_mem = false;
    state.MEM.wrt_enable = false;
    newState.MEM.wrt_enable = false;
    state.MEM.nop = true;
    newState.MEM.nop = true;
    state.WB.wrt_enable=false;
    newState.WB.wrt_enable = false;
    state.WB.nop = true;
    newState.WB.nop=true;


    while (1) {

  /* --------------------- WB stage --------------------- */

if (state.WB.nop)

    {

    }
else
    {
    if (state.WB.wrt_enable)
        {
            myRF.writeRF(state.WB.Wrt_reg_addr,state.WB.Wrt_data);
            if (state.WB.Wrt_reg_addr==state.EX.Rs)   // MEM-EX FORWARDING
                    {
                        state.EX.Read_data1 = state.WB.Wrt_data; // check here - check once - need to double check
                    }
            else if (state.WB.Wrt_reg_addr==state.EX.Rt)
                {
                        state.EX.Read_data2 = state.WB.Wrt_data;// check here - check once - need to double check
                }
        if (state.WB.Rt==state.MEM.Rt)
                {
                            state.MEM.Store_data = state.WB.Wrt_data;
                }
        }
    }
        /* --------------------- MEM stage --------------------- */
if ( state.MEM.nop)
{
    newState.WB.Rs= state.MEM.Rs;
    newState.WB.Rt= state.MEM.Rt;
    newState.WB.Wrt_reg_addr=state.MEM.Wrt_reg_addr;
    newState.WB.wrt_enable=state.MEM.wrt_enable;
    newState.WB.Wrt_data = (state.MEM.rd_mem)?newState.WB.Wrt_data:state.MEM.ALUresult;// I want to send the value of register 0, is it exist?
}
else
{
    if (state.MEM.wrt_enable)// EX-EX FORWARDING
        {
            if (state.MEM.Wrt_reg_addr == state.EX.Rs)
                {
                    state.EX.Read_data1 = state.MEM.ALUresult;// check here - check once - need to double check
                }
            else if (state.MEM.Wrt_reg_addr == state.EX.Rt)
                {
                        state.EX.Read_data2 = state.MEM.ALUresult;// check here - check once - need to double check
                }
        }
    if (state.MEM.wrt_mem)
        {
        if(state.MEM.Wrt_reg_addr==state.EX.Rt)
                {
                    state.MEM.Store_data = myRF.readRF(state.EX.Rt);
                }

       {
           myDataMem.writeDataMem(state.MEM.ALUresult,state.MEM.Store_data);
       }
        }
    else if (state.MEM.rd_mem)
       {

           newState.WB.Wrt_data  =    myDataMem.readDataMem(state.MEM.ALUresult);
       }
    newState.WB.Wrt_data    =  (state.MEM.rd_mem)?newState.WB.Wrt_data:state.MEM.ALUresult;
    newState.WB.Rs  =   state.MEM.Rs;
    newState.WB.Rt  =   state.MEM.Rt;
    newState.WB.Wrt_reg_addr    =   state.MEM.Wrt_reg_addr;
    newState.WB.wrt_enable  =   state.MEM.wrt_enable;
}
newState.WB.nop = state.MEM.nop;
        /* --------------------- EX stage --------------------- */
if (state.EX.nop)
    {
                  state.EX.Read_data2 = (state.EX.is_I_type)?signextend (state.EX.Imm):state.EX.Read_data2;
                  newState.MEM.ALUresult=(state.EX.alu_op)?(state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong()):(state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong());
                  newState.MEM.Store_data= myRF.readRF(state.EX.Rt);
                  newState.MEM.Rs= state.EX.Rs;
                  newState.MEM.Rt= state.EX.Rt;
                  newState.MEM.Wrt_reg_addr= state.EX.Wrt_reg_addr;
                  newState.MEM.wrt_enable=state.EX.wrt_enable;
                  newState.MEM.rd_mem=state.EX.rd_mem;
                  newState.MEM.wrt_mem=state.EX.wrt_mem;
    }
else
    {
                  state.EX.Read_data2 = (state.EX.is_I_type)?signextend (state.EX.Imm):state.EX.Read_data2;
                  newState.MEM.ALUresult=(state.EX.alu_op)?(state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong()):(state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong());
                  newState.MEM.rd_mem=state.EX.rd_mem;
                  newState.MEM.Rs= state.EX.Rs;
                  newState.MEM.Rt= state.EX.Rt;
                  newState.MEM.wrt_mem=state.EX.wrt_mem;
                  newState.MEM.Store_data= myRF.readRF(state.EX.Rt);
                  newState.MEM.Wrt_reg_addr= state.EX.Wrt_reg_addr;
                  newState.MEM.wrt_enable=state.EX.wrt_enable;
    }
newState.MEM.nop = state.EX.nop;
        /* --------------------- ID stage --------------------- */
if (state.ID.nop)
    {
                newState.EX.alu_op = (1);
                newState.EX.Rs= state.EX.Rs;
                newState.EX.Rt= state.EX.Rt;
                newState.EX.is_I_type= state.EX.is_I_type;
                newState.EX.wrt_enable= (0);
                newState.EX.wrt_mem = (0);
                newState.EX.rd_mem = (0);
                newState.EX.Wrt_reg_addr = state.EX.Wrt_reg_addr;
                newState.EX.Imm = state.EX.Imm;
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
    }
else
    {
                opcode = bitset<6> (shiftbits(state.ID.Instr, 26));
                newState.EX.is_I_type = (opcode.to_ulong()!=0)?1:0;
                IsBranch = (opcode.to_ulong()==4)?1:0;
                IsLoad   =   (opcode.to_ulong()==35)?1:0;
                IsStore  =  (opcode.to_ulong()==43)?1:0;
                funct = bitset<6> (shiftbits(state.ID.Instr, 0));
                newState.EX.alu_op = (newState.EX.is_I_type==0 && funct.to_ulong()==35)?0:1;
                newState.EX.rd_mem = (IsLoad.to_ulong())?1:0;
                newState.EX.wrt_mem = (IsStore.to_ulong())?1:0;
                newState.EX.wrt_enable = (( opcode.to_ulong()==35|| opcode.to_ulong()==0)?1:0);
                newState.EX.Rs = bitset<5> (shiftbits(state.ID.Instr, 21));
                newState.EX.Rt = bitset<5> (shiftbits(state.ID.Instr, 16));
                newState.EX.Wrt_reg_addr =  (newState.EX.is_I_type)? newState.EX.Rt : bitset<5> (shiftbits(state.ID.Instr, 11));
                newState.EX.Imm = bitset<16> (shiftbits(state.ID.Instr, 0));
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                BranchAddress = newState.EX.Imm.to_ulong();
                if(newState.MEM.rd_mem)
                    if(!newState.EX.is_I_type)
                        if ( newState.MEM.Rt == newState.EX.Rs || newState.MEM.Rt== newState.EX.Rt)
                            {
                                stall = 1;
                                stall1 = 1;
                                cout << cycle<<":" << "stall";
                            }
        }
newState.EX.nop=state.ID.nop;
        /* --------------------- IF stage --------------------- */
if (state.IF.nop)
    {
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
                if (newState.ID.Instr.to_string<char,std::string::traits_type,std::string::allocator_type>()=="11111111111111111111111111111111")
                    {
                        newState.IF.nop=1;
                    }
    }
else
    {
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
            newState.IF.PC = bitset<32> (state.IF.PC.to_ulong()+4);
                if (newState.ID.Instr.to_string<char,std::string::traits_type,std::string::allocator_type>()=="11111111111111111111111111111111")
                    {
                        newState.IF.nop=1;
                        newState.IF.PC = bitset<32> (newState.IF.PC.to_ulong()-4);
                    }
}
newState.ID.nop = newState.IF.nop;
if(!state.ID.nop)
{
    if (IsBranch)
                {
                    if(newState.EX.Read_data1!=newState.EX.Read_data2)
                    {
                      newState.IF.PC = state.IF.PC.to_ulong()+BranchAddr(newState.EX.Imm).to_ulong() ; //CONVERT TO BRANCHADDRESS
                      newState.ID.nop = 1;
                    }
                }
}
if(stall)
{   newState.ID = state.ID;
    newState.IF = state.IF;
    stall = 0;
}
if(stall1)
{
     newState.EX.nop=1;
     stall1 = 0;
}
if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)    break;
/*The end of the cycle and updates the current state with the values calculated in this cycle */
printState(newState, cycle);
state = newState;//print states after executing cycle 0, cycle 1, cycle 2 ...
cycle = cycle +1;
}
myRF.outputRF(); // dump RF;
myDataMem.outputDataMem(); // dump data mem
return 0;
}


