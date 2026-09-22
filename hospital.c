#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to clean up the URL symbols like %28, %29, %20 and +
void decode_url(char *str) {
    char *src = str;
    char *dst = str;
    while (*src) {
        if (*src == '+') {
            *dst = ' ';
        } else if (*src == '%' && *(src + 1) != '\0' && *(src + 2) != '\0') {
            int value;
            sscanf(src + 1, "%2x", &value);
            *dst = (char)value;
            src += 2;
        } else {
            *dst = *src;
        }
        src++;
        dst++;
    }
    *dst = '\0';
}

int main() {
    printf("Content-Type: text/html\n\n");
    char *data = getenv("QUERY_STRING");
    
    char name[100]="", age[10]="", gender[20]="", step[5]="0";
    
    // Safely check which step we are on without breaking the parser
    if(data != NULL) {
        if (strstr(data, "step=1")) {
            strcpy(step, "1");
            sscanf(data, "name=%[^&]&age=%[^&]&gender=%[^&]", name, age, gender);
            decode_url(name); 
        } else if (strstr(data, "step=2")) {
            strcpy(step, "2");
        }
    }

    printf("<html><head>");
    printf("<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@400;600&family=Work+Sans:wght@700&display=swap' rel='stylesheet'>");
    // Updated path to your new structure
    printf("<link rel='stylesheet' href='http://localhost/HOSPITAL_APP/static/style.css'>");
    
    printf("<style>");
    printf(".radio-group { display: flex; flex-direction: row; justify-content: flex-start; gap: 40px; margin: 10px 0 25px 0; padding-left: 5px; }");
    printf(".radio-item { display: flex; align-items: center; gap: 10px; width: auto !important; }");
    printf(".radio-item input[type='radio'] { width: 20px !important; height: 20px !important; margin: 0 !important; cursor: pointer; }");
    printf(".radio-item label { margin: 0 !important; cursor: pointer; font-size: 16px; white-space: nowrap; }");
    printf("</style>");
    
    printf("</head><body>");
    printf("<div class='container'>");

    // STEP 1: Medical History & Doctor Selection
    if (strcmp(step, "1") == 0) {
        printf("<h2>Medical Profile</h2>");
        printf("<p style='margin-bottom:20px;'>Patient: <strong>%s</strong> (%s)</p>", name, age);
        
        printf("<form action='http://localhost/cgi-bin/hospital.cgi' method='get'>");
        
        printf("<input type='hidden' name='name' value='%s'>", name);
        
        printf("<p style='text-align:left; font-weight:600; margin-bottom: 5px;'>History of Diabetes?</p>");
        printf("<div class='radio-group'>");
            printf("<div class='radio-item'><input type='radio' name='diabetes' id='yes' value='Yes'><label for='yes'>Yes</label></div>");
            printf("<div class='radio-item'><input type='radio' name='diabetes' id='no' value='No' checked><label for='no'>No</label></div>");
        printf("</div>");

        printf("<p style='text-align:left; font-weight:600; margin-bottom: 5px;'>Choose Specialist:</p>");
        printf("<select name='doctor'>");
        printf("<option value='Dr. Smith (General)'>Dr. Smith - General Physician ($50)</option>");
        printf("<option value='Dr. Khan (Eye Specialist)'>Dr. Khan - Ophthalmologist ($120)</option>");
        printf("<option value='Dr. Sarah (Bone Specialist)'>Dr. Sarah - Orthopedic ($200)</option>");
        printf("<option value='Dr. Ahmed (Heart Specialist)'>Dr. Ahmed - Cardiologist ($350)</option>");
        printf("<option value='Dr. Zafar (Cancer Specialist)'>Dr. Zafar - Oncologist ($500)</option>");
        printf("</select>");

        printf("<input type='hidden' name='step' value='2'>");
        printf("<button type='submit'>Generate Final Bill</button>");
        printf("</form>");
    } 
    // STEP 2: The Final Invoice & Database Save
    else if (strcmp(step, "2") == 0) {
        char doctor_type[100]="", diabetes[10]="", final_name[100]="";
        
        // Correctly parse the data coming from Step 1's form
        sscanf(data, "name=%[^&]&diabetes=%[^&]&doctor=%[^&]", final_name, diabetes, doctor_type);
        
        decode_url(final_name);
        decode_url(doctor_type);

        int fee = 50;
        if(strstr(doctor_type, "Eye")) fee = 120;
        else if(strstr(doctor_type, "Bone")) fee = 200;
        else if(strstr(doctor_type, "Heart")) fee = 350;
        else if(strstr(doctor_type, "Cancer")) fee = 500;

        // LOCAL DATABASE LOGIC
        // This will create 'patients.txt' inside your C:\xampp\cgi-bin\ folder!
        FILE *file = fopen("patients.txt", "a+");
        if (file != NULL) {
            fprintf(file, "Name: %-20s | Diabetes: %-5s | Doctor: %-30s | Fee: $%d\n", 
                    final_name, diabetes, doctor_type, fee);
            fclose(file);
        }

        printf("<h2>Final Patient Bill</h2>");
        printf("<div style='text-align:left; line-height:2.0;'>");
            printf("<p><strong>Patient:</strong> %s</p>", final_name);
            printf("<p><strong>Diabetes:</strong> %s</p>", diabetes);
            printf("<p><strong>Specialist:</strong> %s</p>", doctor_type);
            printf("<hr style='border:1px solid #eee; margin:15px 0;'>");
            printf("<h3 style='color:#004d66; text-align:center;'>Total Bill: $%d</h3>", fee);
        printf("</div>");
        
        // Updated Return link pointing back to the templates folder
        printf("<br><a href='http://localhost/HOSPITAL_APP/templates/index.html' style='display:block; text-align:center; color:#0fc7f5; text-decoration:none; font-weight:600;'>+ New Entry</a>");
    }

    printf("</div></body></html>");
    return 0;
}