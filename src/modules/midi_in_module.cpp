#include "midi_in_module.h"

double midi2freq(int n)
{
    double f = 440.0 * pow(2, ((double)n - 69) / 12.0);

    return f;
}

void midiCallback(double deltaTime, std::vector<unsigned char> *message, void *pUserData)
{
//
//    for(int i = 0; i < message->size(); i++)
//    {
//        print("index",i,"data",(int)message->at(i));
//    }
    
    if (message->size() < 3)
    {
        return;
    }
    
    unsigned char command = message->at(0);
    unsigned char noteNum = message->at(1);
    unsigned char velocity = message->at(2);

    std::cout << (int)command << " " << (int)noteNum << " " << (int)velocity << std::endl;

//    UserData *userData = static_cast<UserData *>(pUserData);
    midi_in_module *midi_module = static_cast<midi_in_module *>(pUserData);
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

midi_in_module::midi_in_module(audio_graph<xmodule*>& graph, ImVec2 click_pos) : xmodule(graph, click_pos)
{
    name = module_midi_in__get_name();
    config(0,1);
    
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
             port_names.push_back(portName);
         }
         catch ( RtMidiError &error ) {
         error.printMessage();
         }
         std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
     }

//     midiin_ptr->openPort(1);
     midiin_ptr->setCallback(&midiCallback, this);
    
//    int* secret_luv_uu = new int(123456789);
    input_void_ptr = (void*)&current_midi_note;
    
    ImNodes::SetNodeScreenSpacePos(id, click_pos);
};

void midi_in_module::process()
{
//     std::cout << "id " << id << " midi in process" << std::endl;
    
    input_notes.clear();
    
    while (true) {
        MidiNoteMessage note;
        bool hasNotes = notesQueue.try_dequeue(note);
        if (!hasNotes) {
            break;
        }
//        std::cout << note.isNoteOn << " " << note.noteNum << " " << note.velocity << std::endl;
        current_midi_note = note.noteNum;
        input_notes.push_back(note);
    }
        
};

xmodule* module_midi_in__create(audio_graph<xmodule*>& graph, ImVec2 click_pos)
{
    return new midi_in_module(graph, click_pos);
}

std::string module_midi_in__get_name()
{
    return "midi in";
}

void midi_in_module::save_state(nlohmann::json &object)
{
    
};

void midi_in_module::load_state(nlohmann::json &object)
{
    
};
