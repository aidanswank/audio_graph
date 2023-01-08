#include "midi_in_module.h"

void midiCallback(double deltaTime, std::vector<unsigned char> *message, void *pUserData)
{
    
    if (message->size() < 3)
    {
        return;
    }
    
    unsigned char command = message->at(0);
    unsigned char noteNum = message->at(1);
    unsigned char velocity = message->at(2);

    std::cout << (int)command << " " << (int)noteNum << " " << (int)velocity << std::endl;

//    UserData *userData = static_cast<UserData *>(pUserData);
    rt_midi_in *midi_module = static_cast<rt_midi_in *>(pUserData);
//
////    module_data->notes.clear();
//
////    print("num consumers", module_data->graph->node(module_data->node_id));
//
    if (command == 144) {
        MidiNoteMessage noteOnMsg;
        noteOnMsg.noteNum = noteNum;
        noteOnMsg.velocity = static_cast<float>(velocity) / 127.0f;
        noteOnMsg.isNoteOn = true;
        midi_module->notesQueue.enqueue(noteOnMsg);
    } else if (command == 128) {
        MidiNoteMessage noteOffMsg;
        noteOffMsg.noteNum = noteNum;
        noteOffMsg.velocity = static_cast<float>(velocity) / 127.0f;
        noteOffMsg.isNoteOn = false;
        midi_module->notesQueue.enqueue(noteOffMsg);
    }

};

rt_midi_in::rt_midi_in(audio_graph<xmodule*>& graph) : xmodule(graph)
{
//    name = "rt midi in";
//    graph.push_unique_str(name);
    config("rt midi in",0,1);
    
     try
     {
         midiin_ptr = new RtMidiIn();
     }
     catch (RtMidiError &err)
     {
         err.printMessage();
         // return 1;
     }

     unsigned int nPorts = midiin_ptr->getPortCount();
     if (nPorts == 0)
     {
         std::cout << "No ports available!\n";
     }

     std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
     std::string portName;

     for ( unsigned int i=0; i<nPorts; i++ ) {
         try {
         portName = midiin_ptr->getPortName(i);
         }
         catch ( RtMidiError &error ) {
         error.printMessage();
         }
         std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
     }

     midiin_ptr->openPort(1);
     midiin_ptr->setCallback(&midiCallback, this);
};

void rt_midi_in::process()
 {
//     std::cout << "id " << id << " midi in process" << std::endl;
    
    notes.clear();
    
    while (true) {
        MidiNoteMessage note;
        bool hasNotes = notesQueue.try_dequeue(note);
        if (!hasNotes) {
            break;
        }
        std::cout << note.isNoteOn << " " << note.noteNum << " " << note.velocity << std::endl;
        notes.push_back(note);
    }
        
//    value_stack.push(module_data);
 };
