import cv2
import dlib
from scipy.spatial import distance

# Function to calculate Eye Aspect Ratio (EAR)
def calculate_ear(eye):
    A = distance.euclidean(eye[1], eye[5])
    B = distance.euclidean(eye[2], eye[4])
    C = distance.euclidean(eye[0], eye[3])
    ear = (A + B) / (2.0 * C)
    return ear

# Thresholds and constants
EYE_AR_THRESH = 0.25
EYE_AR_CONSEC_FRAMES = 3

# Initialize counters
blink_counter = 0
frame_counter = 0

# Load face detector and facial landmarks predictor
detector = dlib.get_frontal_face_detector()
#predictor = dlib.shape_predictor("E:\Robotics & Mechatronics engineering\Research and Experiments\IDAS-PC\Eyeblink-to-ESP32-Python/shape_predictor_68_face_landmarks.dat")
predictor = dlib.shape_predictor("Eyeblink-to-ESP32-Python/shape_predictor_68_face_landmarks.dat")

# Start video capture
cap = cv2.VideoCapture(1)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = detector(gray)

    for face in faces:
        landmarks = predictor(gray, face)

        # Extract eye landmarks
        left_eye = []
        right_eye = []
        for i in range(36, 42):  # Left eye
            left_eye.append((landmarks.part(i).x, landmarks.part(i).y))
        for i in range(42, 48):  # Right eye
            right_eye.append((landmarks.part(i).x, landmarks.part(i).y))

        # Calculate EAR for both eyes
        left_ear = calculate_ear(left_eye)
        right_ear = calculate_ear(right_eye)
        ear = (left_ear + right_ear) / 2.0

        # Draw eye landmarks
        for point in left_eye + right_eye:
            cv2.circle(frame, point, 2, (0, 255, 0), -1)

        # Check if EAR is below the threshold
        if ear < EYE_AR_THRESH:
            frame_counter += 1
        else:
            if frame_counter >= EYE_AR_CONSEC_FRAMES:
                blink_counter += 1
            frame_counter = 0

        # Display EAR and blink count
        cv2.putText(frame, f"EAR: {ear:.2f}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)
        cv2.putText(frame, f"Blinks: {blink_counter}", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)

    # Show the frame
    cv2.imshow("Eye Blink Counter", frame)

    # Break the loop on 'q' key press
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release resources
cap.release()
cv2.destroyAllWindows()